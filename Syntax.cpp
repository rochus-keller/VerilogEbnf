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

#include "Syntax.h"
#include <QTextStream>
#include <QtDebug>
#include <Verilog/VlToken.h>
using namespace Vl;

// Aus Ada::Syntax adaptiert

Syntax::Syntax(QObject *parent) :
    QObject(parent)
{

}

Syntax::~Syntax()
{
	clearDefs();
}

const Syntax::Node *Syntax::nodeFromDef(const QString & name)
{
	Definitions::const_iterator i = d_defs.find( name );
	if( i != d_defs.end() && i.value()->d_node != 0 )
		return i.value()->d_node;
	else
		return 0;
}

void Syntax::dump() const
{
	qDebug() << "******** Begin Dump";
	Syntax::Definitions::const_iterator i;
	for( i = d_defs.begin(); i != d_defs.end(); ++i )
		i.value()->dump();
	qDebug() << "******** End Dump";
}



void Syntax::initDefs(bool presetPrimitives)
{
    d_error.clear();
	clearDefs();
	Definition* d;

	if( presetPrimitives )
	{
        d = new Definition();
        d->d_name = "identifier";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_Ident );

        d = new Definition();
        d->d_name = "system_name";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_SysName );

        d = new Definition();
        d->d_name = "real_number";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_Realnum );

        d = new Definition();
        d->d_name = "natural_number";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_Natural );

        d = new Definition();
        d->d_name = "sizedbased_number";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_SizedBased );

        d = new Definition();
        d->d_name = "based_number";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_BasedInt );

        d = new Definition();
        d->d_name = "base_format";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_BaseFormat );

        d = new Definition();
        d->d_name = "base_value";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_BaseValue );

        d = new Definition();
        d->d_name = "string";
        addDef( d );
        d->d_node = new Node(Node::Terminal, d, Tok_String );

        d = new Definition();
        d->d_name = "unary_operator";
        addDef( d );
        d->d_node = new Node(Node::Alternative, d );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Plus ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Minus ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bang ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Tilde ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeAmp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeBar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Hat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeHat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_HatTilde ) );

        d = new Definition();
        d->d_name = "binary_operator";
        addDef( d );
        d->d_node = new Node(Node::Alternative, d );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Plus ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Minus ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Star ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Slash ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Percent ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Eq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_BangEq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_3Eq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bang2Eq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Star ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Lt ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Leq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Gt ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Geq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Hat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_HatTilde ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeHat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Gt ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Lt ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_3Gt ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_3Lt ) );

        d = new Definition();
        d->d_name = "unary_module_path_operator";
        addDef( d );
        d->d_node = new Node(Node::Alternative, d );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bang ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Tilde ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeAmp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeBar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Hat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeHat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_HatTilde ) );

        d = new Definition();
        d->d_name = "binary_module_path_operator";
        addDef( d );
        d->d_node = new Node(Node::Alternative, d );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Eq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_BangEq ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_2Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Amp ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Bar ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_Hat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_TildeHat ) );
        d->d_node->d_subs.append( new Node(Node::Terminal, d, Tok_HatTilde ) );
    }
}

void Syntax::clearDefs()
{
	foreach( Definition* d, d_defs )
		delete d;
    d_defs.clear();
    d_order.clear();
}

bool Syntax::addDef(Syntax::Definition* d)
{
    d_error.clear();
    if( d_defs.contains( d->d_name ) )
    {
        d_error = tr("cannot add duplicate definition");
        return false;
    }else
    {
        d_defs.insert( d->d_name, d );
        d_order.append( d );
        return true;
    }
}

const Syntax::Definition*Syntax::getDef(const QString& name) const
{
    return d_defs.value(name);
}

const Syntax::Node*Syntax::getRootNode(const QString& name) const
{
    const Definition* def = getDef(name);
    if( def )
        return def->d_node;
    else
        return 0;
}

bool Syntax::checkRefs()
{
    d_error.clear();
	Syntax::Definitions::const_iterator i;
	for( i = d_defs.begin(); i != d_defs.end(); ++i )
	{
		i.value()->d_usedBy.clear();
        i.value()->d_derefed = false;
	}
	for( i = d_defs.begin(); i != d_defs.end(); ++i )
	{
        if( !i.value()->d_derefed )
        {
            i.value()->d_derefed = true;
            Node* node = i.value()->d_node;
            if( node && !checkRefs( node ) )
                return false;
        }
	}
	return true;
}

bool Syntax::checkRefs(Syntax::Node *node)
{
	Q_ASSERT( node->d_owner != 0 );
    if( node->d_type == Node::DefRef )
	{
        // Wieso: node->d_subs.clear();
        Q_ASSERT( node->d_subs.isEmpty() );
		Definitions::const_iterator i = d_defs.find( node->d_name );
		if( i != d_defs.end() )
		{
			Definition* def = i.value();
            //Q_ASSERT( def->d_node );
			def->d_usedBy.insert(node);
		}else
		{
            d_error = tr("'%1' on line %2 references unknown '%3'").arg(node->d_owner->d_name).
                    arg(node->d_owner->d_lnr).arg(node->d_name);
			return false;
		}
	}else
	{
		foreach( Node* sub, node->d_subs )
		{
			if( !checkRefs( sub ) )
				return false;
		}
	}
	return true;
}

void Syntax::uncheckRefs( Syntax::Node* node )
{
    if( node->d_type == Syntax::Node::DefRef )
	{
		Definition* def = d_defs.value( node->d_name );
		if( def != 0 )
		{
			if( !def->d_usedBy.contains(node) )
				qWarning() << "uncheckRefs: node already removed from def" << node->d_owner->d_name << node->d_name;
			def->d_usedBy.remove(node);
		}else
			qWarning() << "uncheckRefs: cannot resolve ref" << node->d_owner->d_name << node->d_name;
	}else
	{
		foreach( Syntax::Node* sub, node->d_subs )
			uncheckRefs( sub );
    }
}

Syntax::NodeSet Syntax::calcFirstSet(QList<const Syntax::Node*>& stack)
{
    NodeSet res;
    if( stack.isEmpty() )
        return res;
    const NodeSet ns = stack.back()->d_first;
    foreach( Syntax::Node* n, ns )
    {
        if( n == 0 )
            ; // nop
        else if( n->d_type == Syntax::Node::Terminal || n->d_type == Syntax::Node::Literal )
            res.insert( n );
        else
        {
            if( !stack.contains(n) )
            {
                stack.push_back(n);
                res += calcFirstSet(stack);
                stack.pop_back();
            }
        }
    }
    return res;
}

void Syntax::uncheckRefs(Syntax::Definition *def)
{
	Q_ASSERT( def != 0 );
	uncheckRefs( def->d_node );
}

void Syntax::Definition::dump() const
{
    qDebug() << "###Category '" << d_name << "' line " << d_lnr << d_label << ":";
	if( d_node )
		d_node->dump(1);
	else
		qDebug() << "    No nodes";
}

Syntax::Node::~Node()
{
	foreach( Node* n, d_subs )
		delete n;
}

void Syntax::Node::dump(int level) const
{
	QString str;
	switch( d_type )
	{
	case Terminal:
        str = QString("Terminal '%1'").arg( tokenToString( d_lexTok ) );
		break;
    case DefRef:
        str = QString("Category '%1'").arg( d_name );
		break;
	case Literal:
        str = QString("Literal '%1'").arg( d_name );
		break;
	case Alternative:
		str = "alt";
		break;
	case Sequence:
		str = "seq";
		break;
	}
	switch( d_quant )
	{
	case One:
		break;
	case ZeroOrOne:
		str = QString("[%1]").arg(str);
		break;
	case ZeroOrMore:
		str = QString("{%1}").arg(str);
		break;
	case OneOrMore:
		str = QString("{%1}+").arg(str);
		break;
	}

	qDebug() << QString( level * 4, QChar(' ') ) << "$$$" << str;
	foreach( Node* n, d_subs )
		n->dump(level+1);
}

Syntax::Node *Syntax::Node::clone(Syntax::Definition *newOwner) const
{
	Node* clone = new Node(d_type,newOwner);
	clone->d_quant = d_quant;
    clone->d_lexTok = d_lexTok;
	clone->d_name = d_name;
	foreach( Node* sub, d_subs )
		clone->d_subs.append( sub->clone(newOwner) );
    return clone;
}

Syntax::NodeSet Syntax::Node::calcFirstSet() const
{
    QList<const Node*> stack;
    stack.push_back(this);
    return Syntax::calcFirstSet(stack);
}

static Syntax::LlkNodes calcLlkSetImp( const Syntax::Node* node, int k )
{
    Syntax::LlkNodes res;
    if( node == 0 || k == 0 )
        return res;

    switch( node->d_type )
    {
    case Syntax::Node::DefRef:
        if( !node->d_first.isEmpty() )
            return calcLlkSetImp( *node->d_first.begin(), k );
        else
            qWarning() << "calcLlkSetImp: DefRef node has no link to refed root" << node->d_name << "in" << node->d_owner->d_name;
        break;
    case Syntax::Node::Terminal:
    case Syntax::Node::Literal:
        res.append( node->calcFirstSet() );
        // TODO: Cardinality
        break;
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size() && i < k; i++ )
            res.append( node->d_subs[i]->calcFirstSet() );
        // TODO: Cardinality
        break;
    case Syntax::Node::Alternative:
        res.append( node->calcFirstSet() );
        // TODO: Cardinality
        break;
    }

    return res;
}

Syntax::LlkNodes Syntax::Node::calcLlkSet(int k) const
{
    return calcLlkSetImp( this, k );
}
