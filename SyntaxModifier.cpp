/*
* Copyright 2018 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the VerilogEbnf application.
*
* The following is the license that applies to this copy of the
* application. For a license to use the application under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "SyntaxModifier.h"
#include "ParserGenerator.h"
#include <QtDebug>
#include <QMultiMap>
#include <QFile>
#include <QTextStream>
using namespace Vl;

QFile s_out("out.log");

QSet<QByteArray> s_group;

SyntaxModifier::SyntaxModifier(Syntax *syn):d_syn(syn)
{
	Q_ASSERT( syn != 0 );
}

void SyntaxModifier::calcLevelAndLoops( bool findAllPaths)
{
    s_group.clear();

	Q_ASSERT( d_syn != 0 );
    Syntax::Definitions::iterator i;
    for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
    {
        i.value()->d_level = 0;
        i.value()->d_recursive = false;
        i.value()->d_recPath = false;
        i.value()->d_visited = false;
    }
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
	{
		d_defStack.clear();
		d_defStack.push_back( i.value() );
        analyzeLevelAndLoopsImp( i.value()->d_node, findAllPaths );
		d_defStack.pop_back();
    }
    s_out.close();
}

void SyntaxModifier::calcLevelAndRecursionPaths(Syntax::Definition* def)
{
    if( def == 0 )
        return;
    d_defStack.clear();
    Q_ASSERT( d_syn != 0 );
    Syntax::Definitions::iterator i;
    // reset levels and recursive flags
    for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
    {
        i.value()->d_level = -1;
        i.value()->d_recursive = false;
        i.value()->d_recPath = false;
        i.value()->d_visited = false;
    }
    def->d_level = 0;
    d_defStack.clear();
    d_defStack.push_back( def );
    calcLevelAndRecursionPathsImp(def->d_node);
    d_defStack.pop_back();
    s_out.close();
}

void SyntaxModifier::simplify()
{
	QMultiMap<int,Syntax::Definition*> sort;
	Syntax::Definitions::iterator i;
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
		sort.insert( i.value()->d_level, i.value() );
	QMapIterator<int,Syntax::Definition*> j(sort);
	j.toBack();
	while( j.hasPrevious() )
	{
		j.previous();
	}
}

bool SyntaxModifier::denormalizeDef(const QString & defName)
{
	Syntax::Definitions::iterator i = d_syn->d_defs.find( defName );
	if( i == d_syn->d_defs.end() )
		return false;
	Syntax::Definition* def = i.value();
	denormalizeDefImp( def );
	d_syn->uncheckRefs( def );
	delete def;
	d_syn->d_defs.erase( i );
	return true;
}

void SyntaxModifier::denormalizeAllSingles()
{
	QList<Syntax::Definition*> toDenormalize;
	Syntax::Definitions::iterator i;
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
	{
		if( i.value()->d_usedBy.size() == 1 )
			toDenormalize.append( i.value() );
	}
	foreach( Syntax::Definition* def, toDenormalize )
	{
		denormalizeDefImp( def );
	}
	foreach( Syntax::Definition* def, toDenormalize )
	{
		d_syn->uncheckRefs( def );
		d_syn->d_defs.remove( def->d_name );
		delete def;
	}

}

void SyntaxModifier::joinOneOrMore()
{
	Q_ASSERT( d_syn != 0 );
	Syntax::Definitions::iterator i;
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
	{
		joinOneOrMore( i.value()->d_node );
	}
}

void SyntaxModifier::removeAliasses()
{
	QStringList toDenorm;
	Q_ASSERT( d_syn != 0 );
	Syntax::Definitions::iterator i;
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
	{
		switch( i.value()->d_node->d_type )
		{
		case Syntax::Node::Terminal:
		case Syntax::Node::DefRef:
		case Syntax::Node::Literal:
			toDenorm.append( i.key() );
			break;
		}
	}
	foreach( const QString& s, toDenorm )
		denormalizeDef(s);
}

void SyntaxModifier::normalizeAlts()
{
	Q_ASSERT( d_syn != 0 );
	Syntax::Definitions::iterator i;
	for( i = d_syn->d_defs.begin(); i != d_syn->d_defs.end(); ++i )
	{
		normalizeAlts( i.value()->d_node );
	}
}

void SyntaxModifier::analyzeLevelAndLoopsImp(Syntax::Node * node, bool findAllPaths)
{
    if( node == 0 )
        return;
	if( node->d_type == Syntax::Node::DefRef )
	{
		Syntax::Definitions::iterator i = d_syn->d_defs.find( node->d_name );
		if( i != d_syn->d_defs.end() )
		{
            Syntax::Definition* ref = i.value();

            if( ref->d_level < 0 || ref->d_level < d_defStack.size() )
            {
                ref->d_level = d_defStack.size();
            }

            const int pos = d_defStack.indexOf( ref );
			if( pos != -1 )
            {
                if( findAllPaths )
                {
                    QStringList trace;
                    for( int j = pos; j < d_defStack.size(); j++ )
                    {
                        trace << d_defStack[j]->d_name;
                        d_defStack[j]->d_recPath = true;
                    }
                    const QByteArray str = trace.join( QChar(' ') ).toUtf8();
                    // unnötig trace << ref->d_name;
                    if( s_group.contains(str) )
                        return;
                    s_group.insert( str );
                    if( !s_out.isOpen() )
                        s_out.open(QIODevice::Append );
                    //qDebug() << "Recursive trace:" << str;
                    s_out.write(str);
                    s_out.write("\n");
                }else
                {
                    if( ref->d_recursive )
                        return;
                    if( ref->d_recPath )
                        node->d_recursive = true;
                }
                ref->d_recursive = true;
                // node->d_recursive = true;
            }else
            {
                d_defStack.push_back( ref );
                analyzeLevelAndLoopsImp( ref->d_node, findAllPaths );
                d_defStack.pop_back();
            }
        }else
			qWarning() << "Category not found:" << node->d_name;
    }else if( node->d_type == Syntax::Node::Sequence || node->d_type == Syntax::Node::Alternative )
	{
		foreach( Syntax::Node* n, node->d_subs )
            analyzeLevelAndLoopsImp( n, findAllPaths );
    }
}

void SyntaxModifier::calcLevelAndRecursionPathsImp(Syntax::Node* node)
{
    if( node == 0 )
        return;
    if( node->d_type == Syntax::Node::DefRef )
    {
        Syntax::Definitions::const_iterator i = d_syn->d_defs.find( node->d_name );
        if( i != d_syn->d_defs.end() )
        {
            Syntax::Definition* ref = i.value();

//            if( ref->d_recursive )
//                return;

            if( ref->d_level < 0 || d_defStack.size() < ref->d_level  )
            {
                ref->d_level = d_defStack.size();
            }

            const int pos = d_defStack.indexOf( ref );
            if( pos != -1 )
            {
                QStringList trace;
                for( int j = pos; j < d_defStack.size(); j++ )
                {
                    trace << d_defStack[j]->d_name;
                    if( j != pos )
                        d_defStack[j]->d_recPath = true;
                }
                const QByteArray str = trace.join( QChar(' ') ).toUtf8();
                if( s_group.contains(str) )
                    return;
                s_group.insert(str);
                if( !s_out.isOpen() )
                    s_out.open(QIODevice::Append );
                //qDebug() << "Recursive trace:" << str;
                s_out.write(str);
                s_out.write("\n");
                ref->d_recursive = true;
            }else
            {
                if( ref->d_visited )
                    return;
                ref->d_visited = true;

                d_defStack.push_back( ref );
                calcLevelAndRecursionPathsImp( ref->d_node );
                d_defStack.pop_back();
            }
        }else
            qWarning() << "Category not found:" << node->d_name;
    }else if( node->d_type == Syntax::Node::Sequence || node->d_type == Syntax::Node::Alternative )
    {
        foreach( Syntax::Node* sub, node->d_subs )
            calcLevelAndRecursionPathsImp( sub );
    }
}

void SyntaxModifier::joinOneOrMore(Syntax::Node * node)
{
	if( node->d_type == Syntax::Node::Sequence )
	{
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			if( ( i + 1 ) < node->d_subs.size() &&
				node->d_subs[i]->d_type == Syntax::Node::DefRef &&
				node->d_subs[i+1]->d_type == Syntax::Node::DefRef &&
				node->d_subs[i]->d_name == node->d_subs[i+1]->d_name &&
				node->d_subs[i]->d_quant == Syntax::Node::One &&
				node->d_subs[i+1]->d_quant == Syntax::Node::ZeroOrMore )
			{
				d_syn->d_comments.append(QString("joined '%1' : '%2'").arg(node->d_owner->d_name).
										 arg(node->d_subs[i]->d_name));
				node->d_subs[i]->d_quant = Syntax::Node::OneOrMore;
				d_syn->uncheckRefs( node->d_subs[i+1] );
				delete node->d_subs[i+1];
				node->d_subs.removeAt(i+1);
			}
		}
	}
	foreach( Syntax::Node* n, node->d_subs )
		joinOneOrMore( n );
}

void SyntaxModifier::inlineCatRef(Syntax::Node * node)
{
	Q_ASSERT( node != 0 && node->d_type == Syntax::Node::DefRef );
	Syntax::Definition* def = d_syn->d_defs.value( node->d_name );
	Q_ASSERT( def != 0 );

	denormalizeDefImp( node, def );
	if( node->d_type == Syntax::Node::Sequence || node->d_type == Syntax::Node::Alternative )
		node->d_name = def->d_name;
}

void SyntaxModifier::denormalizeDefImp(Syntax::Node* node, Syntax::Definition * def)
{
	Q_ASSERT( node->d_type == Syntax::Node::DefRef );
	Syntax::Node* clone = def->d_node->clone( node->d_owner );
	const quint8 oldNodeQuant = node->d_quant;
	d_syn->uncheckRefs(node);
	*node = *clone;
	switch( oldNodeQuant )
	{
	case Syntax::Node::One:
		node->d_quant = clone->d_quant;
		break;
	case Syntax::Node::ZeroOrMore:
	case Syntax::Node::ZeroOrOne:
	case Syntax::Node::OneOrMore:
		if( oldNodeQuant == clone->d_quant || clone->d_quant == Syntax::Node::One )
			node->d_quant = oldNodeQuant;
		else
			qWarning() << "SyntaxModifier::denormalizeDef: incompatible source and target quantity";
		break;
	default:
		qWarning() << "SyntaxModifier::denormalizeDef: invalid quantitiy";
		break;
	}
	d_syn->checkRefs(node);
	clone->d_subs.clear(); // damit von delete clone nicht gelscht
	delete clone;
}

void SyntaxModifier::denormalizeDefImp(Syntax::Definition * def)
{
	if( def->d_node->d_type == Syntax::Node::Sequence || def->d_node->d_type == Syntax::Node::Alternative )
		def->d_node->d_name = def->d_name;
	d_syn->d_comments.append(QString("replaced '%1' -> %2").arg(def->d_name).arg(ParserGenerator::format(*def).simplified() ) );
	foreach( Syntax::Node* node, def->d_usedBy )
	{
		denormalizeDefImp( node, def );
	}
}

void SyntaxModifier::normalizeAlts(Syntax::Node * node)
{
	if( node->d_type == Syntax::Node::Alternative )
	{
		bool doneAnything = false;
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			Syntax::Node* refNode = node->d_subs[i];
			if( refNode->d_type == Syntax::Node::DefRef || refNode->d_type == Syntax::Node::Terminal ||
				refNode->d_type == Syntax::Node::Literal )
			{
				for( int j = 0; j < node->d_subs.size(); j++ )
				{
					Syntax::Node* other = node->d_subs[j];
					if( refNode != other && refNode->d_name == other->d_name &&
						refNode->d_type == other->d_type && refNode->d_lexTok == other->d_lexTok &&
						refNode->d_quant == other->d_quant )
					{
						// Wir haben ein Duplikat gefunden
						d_syn->uncheckRefs( other );
						delete node->d_subs[j];
						node->d_subs.removeAt(j);
						doneAnything = true;
					}
				}
			}
		}
		if( node->d_subs.size() == 1 )
		{
            // Alle Alternativen bis auf eine wurden zusammengefuehrt; konsolidiere den Rest nach oben.
			Syntax::Node* sub = node->d_subs.first();
			*node = *sub;
			d_syn->uncheckRefs( sub );
			d_syn->checkRefs( node );
			delete sub;
			doneAnything = true;
		}
		if( doneAnything )
			d_syn->d_comments.append(QString("normalized alternative in '%1'").arg(node->d_owner->d_name) );
	}
	foreach( Syntax::Node* n, node->d_subs )
		normalizeAlts( n );
}

