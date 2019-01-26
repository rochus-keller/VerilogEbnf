#ifndef BISONGEN_H
#define BISONGEN_H

/*
* Copyright 2019 Rochus Keller <mailto:me@rochus-keller.ch>
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
#include <QSet>
#include "Syntax.h"

namespace Vl
{
    class BisonGen : public QObject
    {
    public:
        explicit BisonGen(Syntax* syn,QObject *parent = 0);

        void generate(const QString& path , bool buildAst = true);

    protected:
        typedef QSet<const Syntax::Definition*> Selection;
        static QString tokenDef( quint8 );
        static void writeNode( QTextStream& out, Syntax::Node* node, const QString& name );

    private:
        Syntax* d_syn;
    };
}

#endif // BISONGEN_H
