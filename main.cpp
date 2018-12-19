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

#include "SyntaxTreeView.h"
#include <QApplication>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
using namespace Vl;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if( a.arguments().size() > 1 )
    {
        QFileInfo path(a.arguments()[1]);
        SyntaxTreeView* v = new SyntaxTreeView();
        v->setWindowTitle( QString("%1 - %2").arg(path.fileName()).arg( qApp->applicationName() ));
        v->showMaximized();
        v->loadFromFile( path.absoluteFilePath() );

    }else
        return -1;

    return a.exec();
}
