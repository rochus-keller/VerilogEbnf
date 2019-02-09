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

#include "CocoGen.h"
#include "EbnfParser.h"
#include <Verilog/VlToken.h>
#include <QTextStream>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QDir>
using namespace Vl;

CocoGen::CocoGen(Syntax* syn, QObject *parent) : QObject(parent),d_syn(syn)
{
    Q_ASSERT( syn != 0 );
}

QString CocoGen::nodeName( QString name )
{
    if( name.startsWith( QChar('$')))
        name = QLatin1String("dlr_") + name.mid(1);
    if( name.contains(QChar('$')) )
        qWarning() << "Node name with $:" << name;
    return name;
}

QString CocoGen::cocoTokenDef( quint8 t )
{
    return QString("T%1").arg(tokenName(t) + 4);
}

void CocoGen::writeAttrNode( QTextStream& out, Syntax::Node* node, int ll )
{
    if( node->d_type == Syntax::Node::Sequence )
    {
        out << "( ";
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            if( i != 0 )
                out << " && ";
            writeAttrNode( out, node->d_subs[i], i+ll );
        }
        out << ") ";
    }else if( node->d_type == Syntax::Node::Alternative )
    {
        out << "( ";
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            if( i != 0 )
                out << " || ";
            writeAttrNode( out, node->d_subs[i], ll );
        }
        out << ") ";
    }else if( node->d_type == Syntax::Node::Terminal )
    {
        out << "peek(" << ll << ") == _" << cocoTokenDef(node->d_lexTok) << " ";
    }else if( node->d_type == Syntax::Node::DefRef )
    {
        const Syntax::Node* root = d_syn->getRootNode( node->d_name );
        Syntax::NodeSet s;
        if( root )
            s = root->calcFirstSet();
        if( s.isEmpty() )
            out << "peek(" << ll << ") == _" << node->d_name << " ";
        else if( s.size() == 1 )
            out << "peek(" << ll << ") == _" << cocoTokenDef( (*s.begin())->d_lexTok) << " ";
        else
        {
            out << "( ";
            Syntax::NodeSet::const_iterator i;
            for( i = s.begin(); i != s.end(); ++i )
            {
                if( i != s.begin() )
                    out << "|| ";
                out << "peek(" << ll << ") == _" << cocoTokenDef( (*i)->d_lexTok) << " ";
            }
            out << ") ";
        }
    }
}

static bool skipTerm(quint8 t)
{
    return false;
    // NOTE: sync with addTerminal
    switch( t )
    {
    case Tok_Semi:
    case Tok_Hash:
    case Tok_At:
    case Tok_Lbrack:
    case Tok_Rbrack:
        return true;
    default:
        return false;
    }
    // es gibt zuviele unerlässliche Keywords, ohne die Information verlorengeht.
    // gewisse haben sogar die Rolle sowohl als Typ als auch Attribut, z.B. reg
    // Im Endeffekt ist es aufwändiger, sich die Auslassungen zu merken, als einfach alle Terminals 1:1 zu behalten.
    // return tokenIsReservedWord(t);
}

void CocoGen::writeCompressedDefs(QTextStream& out, Syntax::Node* node, const Selection& selection, Stack& stack, int level , bool all)
{
    if( node == 0 )
        return;
    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( level > 0 && node->d_type == Syntax::Node::Alternative )
            out << "( ";
        else if( level > 0 && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << "( ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << "[ ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "{ ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT( false );
        break;
    }

    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        if( skipTerm(node->d_lexTok) )
            out << "<b><em>" << tokenToString(node->d_lexTok) << "</em></b> ";
        else
            out << "<b>" << tokenToString(node->d_lexTok) << "</b> ";
        break;
    case Syntax::Node::Literal:
        // Kommt nie vor
        out << "<em>'" << node->d_name.toHtmlEscaped() << "'</em> ";
        break;
    case Syntax::Node::DefRef:
        {
            const Syntax::Definition* ref = d_syn->getDef( node->d_name );
            Q_ASSERT( ref != 0 );
            const bool skip = !selection.contains(ref);
            if( all || !skip )
            {
                if( skip )
                    out << "<em>";
                out << "<a href=\"#" << node->d_name << "\">";
                out << node->d_name.toHtmlEscaped();
                out << "</a>";
                if( skip )
                    out << "</em> ";
                else
                    out << " ";
            }else if( stack.contains(ref) )
            {
                out << "... ";
            }else
            {
                stack.push_back(ref);
                //out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;";
                writeCompressedDefs( out, ref->d_node, selection, stack, level + 1, all );
                stack.pop_back();
            }
        }
        break;
    case Syntax::Node::Alternative:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            if( i != 0 )
            {
                if( level == 0 )
                    out << "<br>   | ";
                else
                    out << "| ";
            }
            writeCompressedDefs( out, node->d_subs[i], selection, stack, level + 1, all );
        }
        break;
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            writeCompressedDefs( out, node->d_subs[i], selection, stack, level + 1, all );
        }
        break;
    }

    switch( node->d_quant )
    {
    case Syntax::Node::One:
        if( level > 0 && node->d_type == Syntax::Node::Alternative )
            out << ") ";
        else if( level > 0 && node->d_type == Syntax::Node::Sequence && !node->d_name.isEmpty() )
            out << ") ";
        break;
    case Syntax::Node::ZeroOrOne:
        out << "] ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "} ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT(false);
        break;
    }
}

void CocoGen::generateAstHeaderBody(const QString& atgPath, const CocoGen::Selection& selection)
{
    QDir dir = QFileInfo(atgPath).dir();

    QFile header( dir.absoluteFilePath( "VlSynTree.h") );
    header.open( QIODevice::WriteOnly );
    QTextStream hout(&header);
    hout.setCodec("Latin-1");

    hout << "#ifndef __VLSYNTREE__" << endl;
    hout << "#define __VLSYNTREE__" << endl;
    hout << "// This file was automatically generated by VerilogEbnf; don't modify it!" << endl;
    hout << endl;
    hout << "#include <Verilog/VlToken.h>" << endl;
    hout << "#include <QList>" << endl;
    hout << endl;

    hout << "namespace Vl {" << endl;

    hout << endl;

    hout << "\t" << "struct SynTree {" << endl;

    typedef QMap<QString,const Syntax::Definition*> DefSort;
    Selection sel2;
    DefSort sort;
    foreach( const Syntax::Definition* d, d_syn->getDefs() )
    {
        if( d->d_node == 0 || !selection.contains(d) )
        {
            qDebug() << "Not used production" << d->d_name;
            continue;
        }
        const bool skip = checkSkipFromAst(d);
        if( skip )
        {
            qDebug() << "Skipped" << d->d_name << "from AST";
            continue;
        }
        sort.insert( nodeName( d->d_name ), d );
        sel2.insert(d);
    }

    hout << "\t\t" << "enum ParserRule {" << endl;
    hout << "\t\t\t" << "R_First = Tok_Eof + 1," << endl;
    hout << "\t\t\t" << "R_Attribute," << endl;
    hout << "\t\t\t" << "R_MacroUsage," << endl;
    DefSort::const_iterator i;
    for( i = sort.begin(); i != sort.end(); ++i )
    {
        hout << "\t\t\t" << "R_" << i.key() << "," << endl;
    }
    hout << "\t\t\t" << "R_Last" << endl;
    hout << "\t\t" << "};" << endl;

    hout << "\t\t" << "SynTree(quint16 r = Tok_Invalid, const Token& = Token() );" << endl;
    hout << "\t\t" << "SynTree(const Token& t ):d_tok(t){}" << endl;
    hout << "\t\t" << /* "virtual " << */ "~SynTree() { foreach(SynTree* n, d_children) delete n; }" << endl;
    hout << endl;
    hout << "\t\t" << "static const char* rToStr( quint16 r );" << endl;
    hout << endl;
    hout << "\t\t" << "Vl::Token d_tok;" << endl;
    hout << "\t\t" << "QList<SynTree*> d_children;" << endl;
    hout << "\t" << "};" << endl;
    hout << endl;
    hout << "}" << endl;
    hout << "#endif // __VLSYNTREE__" << endl;

    QFile body( dir.absoluteFilePath( "VlSynTree.cpp") );
    body.open( QIODevice::WriteOnly );
    QTextStream bout(&body);
    bout.setCodec("Latin-1");

    bout << "// This file was automatically generated by VerilogEbnf; don't modify it!" << endl;
    bout << "#include \"VlSynTree.h\"" << endl;
    bout << "using namespace Vl;" << endl;
    bout << endl;

    bout << "SynTree::SynTree(quint16 r, const Token& t ):d_tok(r){" << endl;
    bout << "\t" << "d_tok.d_lineNr = t.d_lineNr;" << endl;
    bout << "\t" << "d_tok.d_colNr = t.d_colNr;" << endl;
    bout << "\t" << "d_tok.d_sourcePath = t.d_sourcePath;" << endl;
    // don't copy: bout << "\t" << "d_tok.d_val = t.d_val;" << endl;
    bout << "}" << endl;
    bout << endl;

    bout << "const char* SynTree::rToStr( quint16 r ) {" << endl;
    bout << "\t" << "switch(r) {" << endl;
    bout << "\t\tcase R_Attribute: return \"attribute_instance\";" << endl;
    bout << "\t\tcase R_MacroUsage: return \"text_macro_usage\";" << endl;
    for( i = sort.begin(); i != sort.end(); ++i )
    {
        bout << "\t\tcase R_" << i.key()
            << ": return \"" << i.value()->d_name << "\";" << endl;
    }
    bout << "\tdefault: if(r<R_First) return tokenName(r); else return \"\";" << endl;
    bout << "}}" << endl;

    QFile f( dir.absoluteFilePath( "CompressedSyntax.html") );
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("utf-8");
    out.setCodec( "UTF-8" );
    out << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">" << endl;
    out << "<html><META http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">" << endl;
    out << "<head><title>Verilog05 Compressed Syntax</title>" << endl;
    //writeCss(out);
    out << "<style type=\"text/css\" media=\"screen, projection, print\">" << endl;
    out << "body{font-family:Arial,sans-serif;font-size:medium;orphans:2;widows:2;}" << endl;
    out << "b{color:red}" << endl;
    out << "em{color:green}" << endl;
    out << "a[href^=\"#\"]{text-decoration:none}";
    out << "</style>" << endl;
    out << "</head><body>" << endl;
    out << "<p>This file was automatically generated by VerilogEbnf; don't modify it!</p>" << endl;
    out << "<dl>" << endl;

    bool all = true;

    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        const bool skip = !sel2.contains(d);
        if( !all && skip || !selection.contains(d) )
            continue;
        out << "<dt>";
        if( skip )
            out << "<em>";
        out << "<a name=\"" << d->d_name << "\">";
        out << d->d_name;
        out << "</a>";
        if( skip )
            out << "</em>";
        out << "&nbsp;:</dt>" << endl;
        out << "<dd>";
        Stack stack;
        writeCompressedDefs( out, d->d_node, sel2, stack, 0, all );
        out << "</dd><br>" << endl;
    }
    out << "</dl></body></html>";

}

void CocoGen::writeAttr( QTextStream& out, Syntax::Node* node )
{
    if( node->d_attr.isEmpty() )
        return;

    // node ist eine Sequence!
    if( node->d_attr.startsWith("LL:") )
    {
        const int ll = node->d_attr.mid(3).toInt();
        if( ll <= 1 )
            return;

        Syntax::LlkNodes llkNodes = node->calcLlkSet(ll);
        out << "IF( ";
        for( int i = 0; i < llkNodes.size(); i++ )
        {
            if( i != 0 )
                out << "&& ";
            if( llkNodes[i].size() > 1 )
                out << "( ";
            Syntax::NodeSet::const_iterator j;
            for( j = llkNodes[i].begin(); j != llkNodes[i].end(); ++j )
            {
                if( j != llkNodes[i].begin() )
                    out << "|| ";
                out << "peek(" << i+1 << ") == _" << cocoTokenDef( (*j)->d_lexTok) << " ";
            }
            if( llkNodes[i].size() > 1 )
                out << ") ";
        }
        out << ") ";
    }else if( node->d_attr.startsWith("LA:") )
    {
        AttrParser p;
        Syntax::Node* n = (Syntax::Node*)p.parse( node->d_attr.mid(3) );
        if( n )
        {
            out << "IF( ";
            writeAttrNode(out,n,1);
            out << ") ";
            delete n;
        }
    }
}

void CocoGen::writeNode( QTextStream& out, Syntax::Node* node, bool topLevel, bool buildAst )
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
        out << "[ ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "{ ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT( false );
        break;
    }

    writeAttr(out,node);

    switch( node->d_type )
    {
    case Syntax::Node::Terminal:
        out << cocoTokenDef( node->d_lexTok ) << " ";
        if( buildAst )
            out << "(. addTerminal(); .) ";
        break;
    case Syntax::Node::Literal:
        out << "\'" << node->d_name << "\' ";
        if( buildAst )
            out << "(. addTermianl(); .) ";
        break;
    case Syntax::Node::DefRef:
        out << nodeName(node->d_name) << " ";
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
            writeNode( out, node->d_subs[i], false, buildAst );
        }
        break;
    case Syntax::Node::Sequence:
        for( int i = 0; i < node->d_subs.size(); i++ )
        {
            writeNode( out, node->d_subs[i], false, buildAst );
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
        out << "] ";
        break;
    case Syntax::Node::ZeroOrMore:
        out << "} ";
        break;
    case Syntax::Node::OneOrMore:
        Q_ASSERT(false);
        break;
    }
}

void CocoGen::findAllUsedProductions( Syntax* syn, Syntax::Node* node, Selection& selection )
{
    if( node == 0 )
        return;
    if( node->d_type == Syntax::Node::DefRef )
    {
        const Syntax::Definition* ref = syn->getDef( node->d_name );
        if( ref )
        {
            if( !selection.contains(ref) )
            {
                selection.insert(ref);
                findAllUsedProductions( syn, ref->d_node, selection );
            }
        }else
            qWarning() << "Category not found:" << node->d_name;
    }else if( node->d_type == Syntax::Node::Sequence || node->d_type == Syntax::Node::Alternative )
    {
        foreach( Syntax::Node* sub, node->d_subs )
            findAllUsedProductions( syn, sub, selection );
    }
}

bool CocoGen::isLeafNode( const Syntax::Node* n )
{
    return n->d_type == Syntax::Node::DefRef ||
            n->d_type == Syntax::Node::Terminal ||
            n->d_type == Syntax::Node::Literal;
}

bool CocoGen::isSimpleNode( const Syntax::Node* n )
{
    if( n->d_type == Syntax::Node::Sequence && n->d_subs.size() == 2 )
    {
        if( isLeafNode( n->d_subs[0] ) )
        {
            if( n->d_subs[1]->d_lexTok == Tok_Semi ) // alle "A ;"
                return true;
            if( n->d_subs[1]->d_type == Syntax::Node::Sequence &&
                    n->d_subs[1]->d_subs.size() == 2 &&
                    n->d_subs[1]->d_quant == Syntax::Node::ZeroOrMore &&
                    n->d_subs[1]->d_subs[0]->d_lexTok == Tok_Comma &&
                    n->d_subs[1]->d_subs[1]->d_name == n->d_subs[0]->d_name ) // alle "A { , A }"
                return true;
        }
    }
    return false;
}

bool CocoGen::checkSkipFromAst( const Syntax::Definition* d )
{
//    if( d->d_name == "file_path_spec" )
//        qDebug() << "hit";
    static QSet<QString> skips, keeps;
    if( skips.isEmpty() )
    {
        skips << "constant_primary" << "constant_expression_nlr" << "primary" << "expression_nlr" << "primary_2"
              << "module_path_primary" << "module_path_expression_nlr"
              //<< "number"
              //<< "string"
              << "genvar_primary" << "genvar_expression_nlr" << "event_expression_nlr"
              //<< "identifier" << "module_identifier" << "udp_identifier"
              << "statement_or_null"
               ;
    }
    if( keeps.isEmpty() )
    {
        keeps << "file_path_spec" << "default_clause" << "inst_clause" << "cell_clause"
              << "list_of_ports" // da separater Scope
              << "list_of_port_declarations" // damit port_declarations unterscheiden können ob in Params oder Body
                 ;
    }
    if( keeps.contains( d->d_name) )
        return false;
    if( skips.contains( d->d_name ) )
        return true;

    if( d->d_node->d_subs.isEmpty() && isLeafNode( d->d_node ) )
        return true;
    /*
    if( d->d_node->d_subs.isEmpty() && d->d_node->d_type == Syntax::Node::DefRef
            && !skips.contains( d->d_node->d_name ) )
        return true;

    if( d->d_node->d_subs.isEmpty() && d->d_node->d_type == Syntax::Node::Terminal )
        return true;
        */

    if( isSimpleNode( d->d_node ) )
        return true;

    if( d->d_node->d_type == Syntax::Node::Alternative )
    {
        bool allLeafs = true;
        foreach( const Syntax::Node* n, d->d_node->d_subs )
        {
            if( !isSimpleNode(n) && !isLeafNode(n) )
                allLeafs = false;
        }
        if( allLeafs )
            return true;
    }

    return false;
}

void CocoGen::generateCoco(const QString& path, bool buildAst)
{
    // see http://ssw.jku.at/Coco/

    Selection selection;
    const Syntax::Definition* root = d_syn->getDef("translation_unit");
    if( root )
    {
        selection.insert(root);
        findAllUsedProductions( d_syn, root->d_node, selection );
    }

    QFile f(path);
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    out.setCodec("Latin-1");

    out << "// This file was automatically generated by VerilogEbnf; don't modify it!" << endl;
    if( buildAst )
    {
        generateAstHeaderBody( path, selection );

        out << "#include <QStack>" << endl;
        out << "#include <Verilog/VlSynTree.h>" << endl;
        out << "COMPILER Verilog05" << endl;

        out << endl;


        out << "\tVl::SynTree d_root;" << endl;
        out << "\tQStack<Vl::SynTree*> d_stack;" << endl;

        out << "\t" << "void addTerminal() {" << endl; // NOTE: sync with skipTerm
        /*
        out << "\t\t" << "if( " // nein, siehe skipTerm !Vl::tokenIsReservedWord(d_cur.d_type)
               // nein, da es auch ,, gibt: "&& d_cur.d_type != Vl::Tok_Comma "
               " d_cur.d_type != Vl::Tok_Semi "
               //"&& d_cur.d_type != Vl::Tok_Lpar "
               // nein, da sequential_or_combinatorial_entry "&& d_cur.d_type != Vl::Tok_Rpar "
               "&& d_cur.d_type != Vl::Tok_Lbrack "
               "&& d_cur.d_type != Vl::Tok_Rbrack "
               "&& d_cur.d_type != Vl::Tok_Hash "
               "&& d_cur.d_type != Vl::Tok_At "
               "){" << endl;
               */
        out << "\t\t" << "Vl::SynTree* n = new Vl::SynTree( d_cur ); d_stack.top()->d_children.append(n);" << endl;
        out << "\t}" << endl;

    }
    out << endl;

    out << "TOKENS" << endl;

    int t = 0;
    for( t = Tok_Plus; t < Tok_Comment; t++ )
    {
        out << "  " << cocoTokenDef(t) << endl;
    }
    out << endl;

    out << "PRODUCTIONS" << endl;
    out << endl;

    if( buildAst )
        out << "Verilog05 = (. d_stack.push(&d_root); .) translation_unit (. d_stack.pop(); .) . " << endl << endl;
    else
        out << "Verilog05 = translation_unit . " << endl << endl;

    out << endl;

    foreach( const Syntax::Definition* d, d_syn->getDefsInOrder() )
    {
        if( d->d_node == 0 || !selection.contains(d) )
            continue;
        //qDebug() << "selected:" << d->d_name;
        out << nodeName( d->d_name ) << " = " << endl << "    ";
        const bool skip = checkSkipFromAst(d);
        if( buildAst && !skip )
            out << "(. Vl::SynTree* n = new Vl::SynTree( Vl::SynTree::R_" << nodeName( d->d_name ) <<
                   ", d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); .) ( ";
        writeNode( out, d->d_node, true, buildAst );

        // Aussortieren solche, die nur ein Alias sind, oder nur aus einer Auswahl bestehen
        if( buildAst && !skip )
            out << ") (. d_stack.pop(); .) ";
        out << endl << "    ." << endl << endl;
    }

    out << "END Verilog05 ." << endl;
}

