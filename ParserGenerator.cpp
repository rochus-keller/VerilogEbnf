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

#include "EbnfParser.h"
#include "ParserGenerator.h"
#include "Syntax.h"
#include "CocoGen.h"
#include <Verilog/VlToken.h>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QtDebug>
#include <QFileInfo>
#include <QDir>
using namespace Vl;

ParserGenerator::ParserGenerator(Syntax* syn, QObject *parent) :
	QObject(parent),d_syn(syn)
{
	Q_ASSERT( syn != 0 );
}

static inline QString tokenG( quint8 t )
{
    QString tok = tokenName(t);
    tok = tok.mid(4).toUpper();
    if( !tok.isEmpty() && tok[0].isDigit() )
        tok = QChar('T') + tok;
    return tok;
}

static inline QString nodeNameG( QString name )
{
    if( name.startsWith( QChar('$')))
        name = QLatin1String("dlr_") + name.mid(1);
    if( name.contains(QChar('$')) )
        qDebug() << "Node name with $:" << name;
    return name;
}

static inline QString tokenSyn( quint8 t )
{
    switch( t )
    {
    case Tok_Lbrack:
        return "LBR";
    case Tok_Rbrack:
        return "RBR";
    case Tok_Lbrace:
        return "LCU";
    case Tok_Rbrace:
        return "RCU";
    default:
        return QString::fromLatin1(tokenToString(t));
    }
}


static void writeNodeG( QTextStream& out, Syntax::Node* node, bool topLevel )
{
	if( node == 0 )
		return;
	switch( node->d_quant )
	{
	case Syntax::Node::One:
		if( !topLevel && node->d_type == Syntax::Node::Alternative )
			out << "( ";
		else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
			out << "( ";
		break;
	case Syntax::Node::ZeroOrOne:
	case Syntax::Node::ZeroOrMore:
	case Syntax::Node::OneOrMore:
		out << "( ";
		break;
	}
	switch( node->d_type )
	{
	case Syntax::Node::Terminal:
        out << tokenG( node->d_lexTok ) << " ";
		break;
	case Syntax::Node::Literal:
		out << "\'" << node->d_name << "\' ";
		break;
	case Syntax::Node::DefRef:
        out << nodeNameG(node->d_name) << " ";
		break;
	case Syntax::Node::Alternative:
		if( !node->d_name.isEmpty() ) // Annotation
			out << "[" << node->d_name << "] ";
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			if( i != 0 )
			{
				if( topLevel )
					out << endl << "    | ";
				else
					out << "| ";
			}
			writeNodeG( out, node->d_subs[i], false );
		}
		break;
	case Syntax::Node::Sequence:
		if( !node->d_name.isEmpty() ) // Annotation
			out << "[" << node->d_name << "] ";
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			writeNodeG( out, node->d_subs[i], false );
		}
		break;
	}
	switch( node->d_quant )
	{
	case Syntax::Node::One:
		if( !topLevel && node->d_type == Syntax::Node::Alternative )
			out << ") ";
		else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
			out << ") ";
		break;
	case Syntax::Node::ZeroOrOne:
		out << ")? ";
		break;
	case Syntax::Node::ZeroOrMore:
		out << ")* ";
		break;
	case Syntax::Node::OneOrMore:
		out << ")+ ";
		break;
	}
}

void ParserGenerator::generateAntlr(const QString &path, bool withHeader)
{
	QFile f(path);
	f.open( QIODevice::WriteOnly );
	QTextStream out(&f);
	out.setCodec("Latin-1");

	foreach( const QString& cmt, d_syn->getComments() )
		out << "// " << cmt << endl;
	out << endl;

	if( withHeader )
	{
        out << "grammar Verilog05;" << endl << endl;
		out << "options {" << endl;
        out << "    language = Cpp;" << endl;
        out << "    tokenVocab = VerilogLexer;" << endl;
		out << "}" << endl << endl;
        out << "@parser::namespace { Vl }" << endl << endl;


//        out << "output_symbol : OS ;" << endl;
//        out << "next_state : NS ;" << endl;
//        out << "level_symbol : LS ;" << endl;
//        out << "edge_symbol: ES ;" << endl;
//        out << "file_path_spec: FPS ;" << endl;
//        out << "edge_descriptor: ED ;" << endl;
        //out << "udp_identifier : IDENT ;" << endl;
        //out << "module_identifier : IDENT ;" << endl;

        // out << "translation_unit: source_text EOF ;" << endl;

		out << endl;
    }

    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d == 0 || d->d_node == 0 )
            continue;
        out << nodeNameG(d->d_name) << /* ( (i.value()->d_visited)?" ! ":"" ) << */ " : " << endl << "    ";
        writeNodeG( out, d->d_node, true );
        out << endl << "    ;" << endl << endl;
    }

    if( withHeader )
    {
        QFile f2( QFileInfo(path).absoluteDir().absoluteFilePath("VerilogLexer.tokens") );
        f2.open( QIODevice::WriteOnly );
        QTextStream out2(&f2);
        out2.setCodec("Latin-1");
        int t;
        for( t = Tok_Plus; t < Tok_Comment; t++ )
        {
            out2 << tokenG(t) << " = " << t << endl;
        }

//        out2 << "OS = " << t++ << endl;
//        out2 << "NS = " << t++ << endl;
//        out2 << "LS = " << t++ << endl;
//        out2 << "ES = " << t++ << endl;
//        out2 << "FPS = " << t++ << endl;
//        out2 << "ED = " << t++ << endl;
        out2 << endl;
    }


}

static void writeNodePcct( QTextStream& out, Syntax::Node* node, bool topLevel )
{
    if( node == 0 )
        return;
    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << "( ";
        else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << "( ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << "{ ";
        break;
    case Syntax::Node::ZeroOrMore:
    case Syntax::Node::OneOrMore:
        out << "( ";
        break;
    }
    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        out << tokenG( node->d_lexTok ) << " ";
        break;
    case Syntax::Node::Literal:
        out << "\'" << node->d_name << "\' ";
        break;
    case Syntax::Node::DefRef:
        out << nodeNameG(node->d_name) << " ";
        break;
    case Syntax::Node::Alternative:
        if( !node->d_name.isEmpty() ) // Annotation
            out << "[" << node->d_name << "] ";
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            if( i != 0 )
            {
                if( topLevel )
                    out << endl << "    | ";
                else
                    out << "| ";
            }
            writeNodePcct( out, node->d_subs[i], false );
        }
        break;
    case Syntax::Node::Sequence:
        if( !node->d_name.isEmpty() ) // Annotation
            out << "[" << node->d_name << "] ";
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            writeNodePcct( out, node->d_subs[i], false );
        }
        break;
    }
    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << ") ";
        else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << ") ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << "} ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << ")* ";
        break;
    case Syntax::Node::OneOrMore:
        out << ")+ ";
        break;
    }
}

void ParserGenerator::generatePccts(const QString& path, bool withHeader)
{
    QFile f(path);
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("Latin-1");

    foreach( const QString& cmt, d_syn->getComments() )
        out << "/* " << cmt << " */" << endl;
    out << endl;

    if( withHeader )
    {
        //out << "#header" << endl;

        out << endl;
    }

    //out << "#parser Verilog05" << endl;

    out << "#tokdefs \"Verilog05Tokens.h\"" << endl;


    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d == 0 || d->d_node == 0 )
            continue;
        out << nodeNameG(d->d_name) << /* ( (i.value()->d_visited)?" ! ":"" ) << */ " : " << endl << "    ";
        writeNodePcct( out, d->d_node, true );
        out << endl << "    ;" << endl << endl;
    }

    if( withHeader )
    {
        QFile f2( QFileInfo(path).absoluteDir().absoluteFilePath("Verilog05Tokens.h") );
        f2.open( QIODevice::WriteOnly );
        QTextStream out2(&f2);
        out2.setCodec("Latin-1");
        int t;
        for( t = Tok_Plus; t < Tok_Comment; t++ )
        {
            out2 << "#define " << tokenG(t) << " " << t << endl;
        }

        out2 << endl;
    }

}

static void writeNodeB( QTextStream& out, Syntax::Node* node, const QString& name )
{
    // Konvertiert EBNF in BNF
    // see https://stackoverflow.com/questions/2466484/converting-ebnf-to-bnf
    if( node == 0 )
        return;

    // Syntax::Node::One: einfach hinschreiben
    // Syntax::Node::ZeroOrOne: option mit epsilon
    // Syntax::Node::ZeroOrMore

    const QString myName = name;

    out << myName << " : " << endl << "    ";

    switch( node->d_quant )
    {
    case Syntax::Node::ZeroOrOne:
        out << "%empty " << endl << "    | ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "%empty " << endl << "    | " << myName << " ";
        break;
    }

    int number = 0;
    QList< QPair<Syntax::Node*,QString> > denormalize;

    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        out << tokenG( node->d_lexTok ) << " ";
        break;
    case Syntax::Node::Literal:
        out << "\'" << node->d_name << "\' ";
        break;
    case Syntax::Node::DefRef:
        out << nodeNameG(node->d_name) << " ";
        break;
    case Syntax::Node::Alternative:
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            switch( node->d_subs[i]->d_type )
            {
            case Syntax::Node::Terminal:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << tokenG( node->d_subs[i]->d_lexTok ) << " ";
                break;
            case Syntax::Node::Literal:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << "\'" << node->d_subs[i]->d_name << "\' ";
                break;
            case Syntax::Node::DefRef:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << nodeNameG(node->d_subs[i]->d_name) << " ";
                break;
            case Syntax::Node::Alternative:
            case Syntax::Node::Sequence:
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }
                break;
            }
            if( node->d_type == Syntax::Node::Alternative && i < node->d_subs.size() - 1 )
                out << endl << "    | ";
        }
    }

    out << endl << "    ;" << endl << endl;

    for( int i = 0; i < denormalize.size(); i++ )
    {
        writeNodeB( out, denormalize[i].first, denormalize[i].second );
    }

}

void ParserGenerator::generateBison(const QString& path, bool withHeader)
{
    QFile f(path);
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("Latin-1");


    if( withHeader )
    {
        out << "%{" << endl;
        out << "extern int yylex();" << endl;
        out << "extern void yyerror (char const *);" << endl;
        out << "%}" << endl;

        out << endl;

        int t = 0;
        for( t = Tok_Plus; t < Tok_Comment; t++ )
        {
            out << "%token " << tokenG(t) << " " << t << endl;
        }

//        out << "%token " << "OS " << t++ << endl;
//        out << "%token " << "NS " << t++ << endl;
//        out << "%token " << "LS " << t++ << endl;
//        out << "%token " << "ES " << t++ << endl;
//        out << "%token " << "FPS " << t++ << endl;
//        out << "%token " << "ED " << t++ << endl;
//        out << "%token " << "EOF " << 0 << endl;
        out << endl;

        out << "%start source_text" << endl << endl;
    }

    out << "%%" << endl;
    out << endl;

//    out << "output_symbol : OS ;" << endl << endl;
//    out << "next_state : NS ;" << endl << endl;
//    out << "level_symbol : LS ;" << endl << endl;
//    out << "edge_symbol: ES ;" << endl << endl;
//    out << "file_path_spec: FPS ;" << endl << endl;
//    out << "edge_descriptor: ED ;" << endl << endl;
    //out << "udp_identifier : IDENT ;" << endl << endl;
    //out << "module_identifier : IDENT ;" << endl << endl;

    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d->d_node == 0 )
            continue;
        writeNodeB( out, d->d_node, nodeNameG(d->d_name) );
    }

    out << "%%" << endl;



}

static inline QString tokenC( quint8 t )
{
    if( t == Tok_if )
        return "VL_IF";
    else if( t == Tok_end )
        return "VL_END";
    else
        return tokenG( t );
}

static void writeNodeS( QTextStream& out, Syntax::Node* node, const QString& name )
{
    // Konvertiert EBNF in BNF
    // see https://stackoverflow.com/questions/2466484/converting-ebnf-to-bnf
    if( node == 0 )
        return;

    // Syntax::Node::One: einfach hinschreiben
    // Syntax::Node::ZeroOrOne: option mit epsilon
    // Syntax::Node::ZeroOrMore

    const QString myName = name;

    out << myName << " : " << endl << "    ";

    switch( node->d_quant )
    {
    case Syntax::Node::ZeroOrOne:
        out << "_epsilon_ " << endl << "    ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "_epsilon_ " << endl << "    " << myName << " ";
        break;
    }

    int number = 0;
    QList< QPair<Syntax::Node*,QString> > denormalize;

    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        out << tokenG( node->d_lexTok ) << " ";
        break;
    case Syntax::Node::Literal:
        out << "\'" << node->d_name << "\' ";
        break;
    case Syntax::Node::DefRef:
        out << nodeNameG(node->d_name) << " ";
        break;
    case Syntax::Node::Alternative:
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            switch( node->d_subs[i]->d_type )
            {
            case Syntax::Node::Terminal:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << tokenG( node->d_subs[i]->d_lexTok ) << " ";
                break;
            case Syntax::Node::Literal:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << "\'" << node->d_subs[i]->d_name << "\' ";
                break;
            case Syntax::Node::DefRef:
                if( node->d_subs[i]->d_quant != Syntax::Node::One )
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }else
                    out << nodeNameG(node->d_subs[i]->d_name) << " ";
                break;
            case Syntax::Node::Alternative:
            case Syntax::Node::Sequence:
                {
                    const QString newName = QString( "%1_%2").arg(name).arg(++number);
                    out << newName << " ";
                    denormalize.append( qMakePair(node->d_subs[i],newName) );
                }
                break;
            }
            if( node->d_type == Syntax::Node::Alternative && i < node->d_subs.size() - 1 )
                out << endl << "    | ";
        }
    }

    out << endl << endl;

    for( int i = 0; i < denormalize.size(); i++ )
    {
        writeNodeS( out, denormalize[i].first, denormalize[i].second );
    }

}

void ParserGenerator::generateSlk(const QString& path, bool withHeader)
{
    // SLK http://www.slkpg2.com/

    CocoGen::Selection selection;
    const Syntax::Definition* root = d_syn->getDef("translation_unit");
    if( root )
    {
        selection.insert(root);
        CocoGen::findAllUsedProductions( d_syn, root->d_node, selection );
    }

    QFile f(path);
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("Latin-1");

    out << "/* This file was automatically generated by VerilogEbnf; don't modify it! */" << endl;

    out << "Verilog05: translation_unit" << endl << endl;


    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d->d_node == 0 || !selection.contains(d) )
            continue;
        writeNodeS( out, d->d_node, nodeNameG(d->d_name) );
    }

}

static void writeNodeP( QTextStream& out, Syntax::Node* node, bool topLevel )
{
    if( node == 0 )
        return;
    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << "( ";
        else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << "( ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << "( ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "( ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT( false );
        break;
    }
    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        out << tokenG( node->d_lexTok ) << " ";
        break;
    case Syntax::Node::Literal:
        out << "\'" << node->d_name << "\' ";
        break;
    case Syntax::Node::DefRef:
        out << nodeNameG(node->d_name) << " ";
        break;
    case Syntax::Node::Alternative:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            if( i != 0 )
            {
                if( topLevel )
                    out << endl << "    / ";
                else
                    out << "/ ";
            }
            writeNodeP( out, node->d_subs[i], false );
        }
        break;
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            writeNodeP( out, node->d_subs[i], false );
        }
        break;
    }
    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << ") ";
        else if( !topLevel && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << ") ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << ")? ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << ")* ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT(false);
        break;
    }
}

void ParserGenerator::generatePeg(const QString& path, bool withHeader)
{
    QFile f(path);
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("Latin-1");

    out << "%header {\n"
        "#define PCC_GETCHAR(auxil) lexerGetNextChar(auxil)\n"
        "extern \"C\" { extern int lexerGetNextChar(void*); }\n"
    "}" << endl << endl;

    out << "translation_unit <- source_text / library_text / attribute_instance" << endl;

    if( withHeader )
    {
        int t = 0;
        for( t = Tok_Plus; t < Tok_Comment; t++ )
        {
            if( t != Tok_unsigned && t != Tok_CoDi ) // weil nicht gebraucht
                out << tokenG(t) << " <- '" << t << "'" << endl;
        }
//        out << "OS " << " <- '" << t++ << "'" << endl << endl;
//        out << "NS " << " <- '" << t++ << "'" << endl << endl;
//        out << "LS " << " <- '" << t++ << "'" << endl << endl;
//        out << "ES " << " <- '" << t++ << "'" << endl << endl;
//        out << "FPS " << " <- '" << t++ << "'" << endl << endl;
//        out << "ED " << " <- '" << t++ << "'" << endl << endl;
    }


//    out << "output_symbol <- OS" << endl;
//    out << "next_state <- NS" << endl;
//    out << "level_symbol <- LS" << endl;
//    out << "edge_symbol <- ES" << endl;
//    out << "file_path_spec <- FPS" << endl;
//    out << "edge_descriptor <- ED" << endl;
    //out << "udp_identifier <- IDENT" << endl;
    //out << "module_identifier <- IDENT" << endl;

    out << endl;


    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d == 0 || d->d_node == 0 )
            continue;
        out << nodeNameG( d->d_name ) << "  <-  " << endl << "    ";
        writeNodeP( out, d->d_node, true );
        out << endl << endl;
    }

}

static void writeNodeSyn( QTextStream& out, Syntax::Node* node, bool topLevel )
{
	if( node == 0 )
		return;
	switch( node->d_quant )
	{
	case Syntax::Node::One:
		if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << "[! ";
		break;
	case Syntax::Node::ZeroOrOne:
		out << "[ ";
		break;
	case Syntax::Node::ZeroOrMore:
	case Syntax::Node::OneOrMore:
		out << "{ ";
		break;
	}
	switch( node->d_type )
	{
	case Syntax::Node::Terminal:
        out << tokenSyn( node->d_lexTok ) << "  ";
		break;
	case Syntax::Node::Literal:
		out << "\'" << node->d_name << "\' ";
		break;
	case Syntax::Node::DefRef:
		out << node->d_name << "  ";
		break;
	case Syntax::Node::Alternative:
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			if( i != 0 )
			{
				if( topLevel )
					out << endl << "    | ";
				else
					out << "| ";
			}
			writeNodeSyn( out, node->d_subs[i], false );
		}
		break;
	case Syntax::Node::Sequence:
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			writeNodeSyn( out, node->d_subs[i], false );
		}
		break;
	}
	switch( node->d_quant )
	{
	case Syntax::Node::One:
		if( !topLevel && node->d_type == Syntax::Node::Alternative )
            out << "!] ";
		break;
	case Syntax::Node::ZeroOrOne:
		out << "] ";
		break;
	case Syntax::Node::ZeroOrMore:
		out << "} ";
		break;
	case Syntax::Node::OneOrMore:
		out << "}+ "; // RISK
		break;
	}
}

QString ParserGenerator::format(const Syntax::Definition & def, bool antlr)
{
	if( def.d_node == 0 )
		return QString();

	QString res;
	QTextStream out( &res );
	if( antlr )
		writeNodeG( out, def.d_node, true );
	else
		writeNodeSyn( out, def.d_node, true );
	return res;
}

static inline QString _xOrEmpty( bool on )
{
	if( on )
		return "x";
	else
		return "";
}

bool ParserGenerator::generateLevelList(const QString &path)
{
	QFile f( path );
	if( !f.open(QIODevice::WriteOnly) )
		return false;
	QTextStream out(&f);
	QMultiMap<int,const Syntax::Definition*> sort;
	Syntax::Definitions::const_iterator i;
	for( i = d_syn->getDefs().begin(); i != d_syn->getDefs().end(); ++i )
		sort.insert( i.value()->d_level, i.value() );
	out << "Level\tLnr\tCategory\tKeep\tRecursive" << endl;
	QMapIterator<int,const Syntax::Definition*> j(sort);
	j.toBack();
	while( j.hasPrevious() )
	{
		j.previous();
		out << j.key() << "\t" << j.value()->d_lnr << "\t" << j.value()->d_name << "\t" <<
               _xOrEmpty(j.value()->d_visited) << "\t" <<
			   _xOrEmpty(j.value()->d_recursive)  << endl;
	}
	return true;
}
