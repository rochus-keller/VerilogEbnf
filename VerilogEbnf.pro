#/*
#* Copyright 2018 Rochus Keller <mailto:me@rochus-keller.ch>
#*
#* This file is part of the VerilogEbnf application.
#*
#* The following is the license that applies to this copy of the
#* application. For a license to use the application under conditions
#* other than those described here, please email to me@rochus-keller.ch.
#*
#* GNU General Public License Usage
#* This file may be used under the terms of the GNU General Public
#* License (GPL) versions 2.0 or 3.0 as published by the Free Software
#* Foundation and appearing in the file LICENSE.GPL included in
#* the packaging of this file. Please review the following information
#* to ensure GNU General Public Licensing requirements will be met:
#* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
#* http://www.gnu.org/copyleft/gpl.html.
#*/

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VerilogEbnf
TEMPLATE = app

INCLUDEPATH += .. ../NAF

SOURCES += main.cpp\
    Syntax.cpp \
    AntlrParser.cpp \
    EbnfParser.cpp \
    SyntaxTreeView.cpp \
    SyntaxAnalyzer.cpp \
    SyntaxModifier.cpp \
    ParserGenerator.cpp \
    CocoGen.cpp \
    BisonGen.cpp

HEADERS  += \
    Syntax.h \
    AntlrParser.h \
    EbnfParser.h \
    SyntaxTreeView.h \
    SyntaxAnalyzer.h \
    SyntaxModifier.h \
    ParserGenerator.h \
    CocoGen.h \
    BisonGen.h

include( ../Verilog/Verilog.pri );

!include(../NAF/Gui2/Gui2.pri) {
         message( "Missing NAF Gui2" )
 }
