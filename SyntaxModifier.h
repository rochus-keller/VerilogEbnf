#ifndef SYNTAXMODIFIER_H
#define SYNTAXMODIFIER_H

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
#include "Syntax.h"

namespace Vl
{
	class SyntaxModifier
	{
	public:
		SyntaxModifier(Syntax* syn);
        void calcLevelAndLoops(bool findAllPaths);
        void calcLevelAndRecursionPaths(Syntax::Definition*);
		void simplify();
		bool denormalizeDef(const QString&);
		void denormalizeAllSingles();
		void joinOneOrMore();
		void removeAliasses();
		void normalizeAlts();
		void inlineCatRef( Syntax::Node* );
	protected:
        void analyzeLevelAndLoopsImp(Syntax::Node*, bool findAllPaths);
        void calcLevelAndRecursionPathsImp(Syntax::Node*);
        void joinOneOrMore(Syntax::Node*);
		void denormalizeDefImp(Syntax::Definition*);
		void denormalizeDefImp(Syntax::Node* node, Syntax::Definition * def);
		void normalizeAlts(Syntax::Node*);
	private:
		Syntax* d_syn;
		QList<Syntax::Definition*> d_defStack;
	};
}

#endif // SYNTAXMODIFIER_H
