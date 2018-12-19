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

#include "SyntaxAnalyzer.h"
#include "Syntax.h"
#include <QMultiMap>
#include <QtDebug>
using namespace Vl;

TokenSet TokenSet::operator +(const TokenSet &rhs) const
{
	TokenSet res;
	for( size_t i = 0; i < size(); i++ )
	{
		res.set( i, test(i) || rhs.test(i) );
	}
	return res;
}

bool TokenSet::disjoint(const TokenSet &rhs) const
{
	if( !test(0) || !rhs.test(0) )
		return true;
	int count = 0;
	for( size_t i = 0; i < size(); i++ )
	{
		if( test(i) && rhs.test(i) ) // Wenn das Symbol in beiden vorhanden, knnen wir nicht entscheiden
			return false;
		if( test(i) != rhs.test(i) )
			count++;
	}
	return count != 0;
}

SyntaxAnalyzer::SyntaxAnalyzer(Syntax* syn ):d_syn(syn)
{
}

void SyntaxAnalyzer::calcFirstSets()
{
    Q_ASSERT( d_syn );
    foreach( Syntax::Definition* def, d_syn->getDefs() )
        calcFirstSets( def->d_node );
}

SyntaxAnalyzer::NodeSet SyntaxAnalyzer::calcFirstSets(Syntax::Node* node)
{
    if( node == 0 )
        return NodeSet();
    if( !node->d_first.isEmpty() )
        return node->d_first;
    switch( node->d_type )
    {
    case Syntax::Node::Sequence:
        {
            // Gehe einmal ganz durch die Sequenz um alle Elemente zu berechnen
            foreach( Syntax::Node* sub, node->d_subs )
                calcFirstSets( sub );

            NodeSet res;
            // gehe durch die Sequence und verwende die Symbole des ersten nicht optionalen Elements
            foreach( Syntax::Node* sub, node->d_subs )
            {
                switch( sub->d_quant )
                {
                case Syntax::Node::One:
                case Syntax::Node::OneOrMore:
                    res += calcFirstSets( sub );
                    node->d_first += res;
                    return res; // Das war das erste nicht optionale
                case Syntax::Node::ZeroOrMore:
                case Syntax::Node::ZeroOrOne:
                    res += calcFirstSets( sub ); // die können alle auch vorkommen da optional
                    break;
                }
            }
            // TODO: analysiere auch das Innere der Sequenz im Falle von optionalen Teilen, wie eindeutig entschieden
            // werden kann, ob diese vorhanden sind oder nicht
            node->d_first += res;
            return res;
            // qWarning() << "SyntaxAnalyzer::calcPaths: sequence with indefinite end in " << node->d_owner->d_name;
        }
        break;
    case Syntax::Node::Alternative:
        {
            NodeSet res;
            foreach( Syntax::Node* sub, node->d_subs )
                res += calcFirstSets( sub );
            node->d_first += res;
//            if( node != node->d_owner->d_node )
//                qDebug() << "NON ROOT ALT" << node->d_owner->d_name; // renders net_declaration with orig syntax
            return res;
        }
        break;
    case Syntax::Node::Terminal:
    case Syntax::Node::Literal:
        {
            NodeSet res;
            res.insert(node);
            node->d_first += res;
            return res;
        }
        break;
    case Syntax::Node::DefRef:
        {
            Q_ASSERT( d_syn );
            Syntax::Definition* def = d_syn->getDefs().value( node->d_name );
            if( def != 0 )
            {
                NodeSet res;
                Syntax::Node* noderef = def->d_node;
                res.insert( noderef );
                node->d_first += res;
                // die einzige Möglichkeit, wie ein anderer Node als Teminal oder Literal reinkommt
                return res;
            }else
                qWarning() << "SyntaxAnalyzer::calcPaths: not found category" << node->d_name << " in " << node->d_owner->d_name;
        }
        break;
    }
    return NodeSet();
}

bool SyntaxAnalyzer::PathTree::operator <(const PathTree &rhs) const
{
	if( d_node == rhs.d_node )
		return false;
	if( d_node == 0 || rhs.d_node == 0 )
		return false;
	if( d_node->d_type != rhs.d_node->d_type )
		return false;
	switch( d_node->d_type )
	{
	case Syntax::Node::Terminal:
		return d_node->d_lexTok < rhs.d_node->d_lexTok;
    case Syntax::Node::DefRef:
	case Syntax::Node::Literal:
		return d_node->d_name < rhs.d_node->d_name;
	default:
		return false;
	}
	return false;
}

bool SyntaxAnalyzer::PathTree::operator ==(const PathTree &rhs) const
{
	if( d_node == rhs.d_node )
		return true;
	if( d_node == 0 || rhs.d_node == 0 )
		return false;
	if( d_node->d_type != rhs.d_node->d_type )
		return false;
	switch( d_node->d_type )
	{
	case Syntax::Node::Terminal:
		return d_node->d_lexTok == rhs.d_node->d_lexTok;
    case Syntax::Node::DefRef:
	case Syntax::Node::Literal:
		return d_node->d_name == rhs.d_node->d_name;
	default:
		return false;
	}
	return false;
}

