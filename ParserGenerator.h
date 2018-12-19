#ifndef ADAPARSERGENERATOR_H
#define ADAPARSERGENERATOR_H

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

#include <QObject>
#include "Syntax.h"

namespace Vl
{
	class ParserGenerator : public QObject
	{
	public:

		explicit ParserGenerator(Syntax* syn, QObject *parent = 0);
		void generateAntlr( const QString& path, bool withHeader = false );
        void generateBison( const QString& path, bool withHeader = false );
        void generateSlk( const QString& path, bool withHeader = false );
        void generatePeg( const QString& path, bool withHeader = false );
        static QString format( const Syntax::Definition&, bool antlr = false );
		bool generateLevelList( const QString& path );
	private:
		Syntax* d_syn;
	};
}

#endif // ADAPARSERGENERATOR_H
