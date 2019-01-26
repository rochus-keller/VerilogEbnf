#ifndef COCOGEN_H
#define COCOGEN_H

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
#include <QSet>
#include "Syntax.h"

namespace Vl
{
    class CocoGen : public QObject
    {
    public:
        typedef QSet<const Syntax::Definition*> Selection;

        explicit CocoGen(Syntax* syn, QObject *parent = 0);

        void generateCoco(const QString& path , bool buildAst = true);

        static bool isSimpleNode( const Syntax::Node* n );
        static inline bool isLeafNode( const Syntax::Node* n );
        static bool checkSkipFromAst( const Syntax::Definition* d );
        static void findAllUsedProductions(Syntax* syn, Syntax::Node* node, Selection& selection );
        static inline QString nodeName( QString name );
    protected:
        static inline QString cocoTokenDef( quint8 t );
        void writeNode( QTextStream& out, Syntax::Node* node, bool topLevel, bool buildAst );
        void writeAttr( QTextStream& out, Syntax::Node* node);
        void writeAttrNode( QTextStream& out, Syntax::Node* node, int ll );
        void generateAstHeaderBody(const QString& atgPath, const Selection& );
        typedef QList<const Syntax::Definition*> Stack;
        void writeCompressedDefs(QTextStream& out, Syntax::Node* node,
                                 const Selection& selection, Stack& stack, int level, bool all );

    private:
        Syntax* d_syn;
    };
}

#endif // COCOGEN_H
