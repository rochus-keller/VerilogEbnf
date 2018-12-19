#ifndef EBNFPARSER_H
#define EBNFPARSER_H

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

class QIODevice;

namespace Vl
{
    class Syntax;

    class EbnfParser : public QObject
    {
    public:
        EbnfParser(QObject* = 0);

        bool parse( QIODevice*, Syntax* );
        const QString& getError() const { return d_error; }
    protected:
        QByteArray readLine();
        bool error(const char* , int lnr);
        char peekChar();

    private:
        QIODevice* d_in;
        QString d_error;
        QByteArray d_curLabel;
        quint16 d_lnr;
        Syntax* d_syn;
    };

    class AttrParser
    {
    public:
        struct Attr;
        void* parse( const QByteArray& line );
        const QString& getError() const { return d_error; }
    private:
        QString d_error;
    };
}

#endif // EBNFPARSER_H
