#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

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

#include <bitset>
#include <QSet>
#include <QHash>
#include <Verilog/VlToken.h>
#include "Syntax.h"

namespace Vl
{
    class TokenSet : public std::bitset<Tok_Eof>
	{
	public:
		TokenSet() {}
		TokenSet operator+( const TokenSet& rhs ) const; // Union
		bool disjoint( const TokenSet& rhs ) const;
	};

	class SyntaxAnalyzer
	{
	public:
		struct PathTree
		{
			Syntax::Node* d_node;
            QList<PathTree> d_firstSet;
			PathTree(Syntax::Node* n = 0):d_node(n){}
			bool operator<( const PathTree& rhs ) const;
			bool operator==( const PathTree& rhs ) const;
		};
        typedef QSet<Syntax::Node*> NodeSet;
        typedef QHash<Syntax::Node*,NodeSet> NodeSets;

        SyntaxAnalyzer(Syntax* syn = 0);

        void calcFirstSets();

	protected:
        NodeSet calcFirstSets( Syntax::Node* );

	private:
		Syntax* d_syn;
        QList<Syntax::Node*> d_nodeStack;
	};
}

#endif // SYNTAXANALYZER_H
