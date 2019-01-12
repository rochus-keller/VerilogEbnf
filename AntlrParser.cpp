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

#include "AntlrParser.h"
#include "Syntax.h"
#include <Verilog/VlToken.h>
#include <QTextStream>
using namespace Vl;

// Aus Ada::Syntax adaptiert

class GeLexer
{
public:
    enum TokenType { Invalid, Ident, Literal, Annotation, Colon, Exclam, Semicolon, Bar, LParen, RParen, Star, Plus, Question, End };
    struct Token
    {
        TokenType d_type;
        quint16 d_lnr;
        QString d_name;
        Token(TokenType t = Invalid ):d_type(t),d_lnr(0){}
        QString print() const
        {
            switch(d_type)
            {
            case Invalid:
                return "###";
            case Ident:
                return d_name;
            case Annotation:
                return QString("[%1]").arg(d_name);
            case Literal:
                return QString("\'%1\'").arg(d_name);
            case Colon:
                return ":";
            case Exclam:
                return "!";
            case Semicolon:
                return ";";
            case Bar:
                return "|";
            case LParen:
                return "(";
            case RParen:
                return ")";
            case Star:
                return "*";
            case Plus:
                return "+";
            case Question:
                return "?";
            default:
                return "<invalid>";
            }
        }
    };
    Token nextToken()
    {
        skipWhiteSpace();
        while( d_pos >= d_line.size() )
        {
            if( d_in->atEnd() )
                return token(End);
            nextLine();
            skipWhiteSpace();
        }
        Q_ASSERT( d_pos < d_line.size() );
        while( d_pos < d_line.size() )
        {
            const QChar ch = d_line[d_pos];
            const ushort ucs = ch.unicode();
            switch( ucs )
            {
            case '(':
                return token( LParen );
            case ')':
                return token( RParen );
            case '*':
                return token( Star );
            case '+':
                return token( Plus );
            case '?':
                return token(Question);
            case ':':
                return token( Colon );
            case '!':
                return token( Exclam );
            case ';':
                return token( Semicolon );
            case '\'':
                return string();
            case '[':
                return annotation();
            case '|':
                return token( Bar );
            default:
                break;
            }
            if( ch.isLetter() )
            {
                // Identifier oder Reserved Word
                return ident();
            }else
            {
                // Error
                return token( Invalid, 1, QString("unexpected character '%1'").arg(ch) );
            }
        }
        Q_ASSERT( false );
        return Invalid;
    }
    const QStringList& getComments() const { return d_comments; }
    GeLexer( QTextStream* in ):d_in(in),d_pos(0),d_lnr(0){}
protected:
    void skipWhiteSpace()
    {
        while( d_pos < d_line.size() )
        {
            if(  d_line[d_pos].isSpace() )
                d_pos++;
            else if( d_line[d_pos] == QChar('/') )
            {
                if( int( d_pos + 1 ) < d_line.size() && d_line[ d_pos + 1 ] == QChar('/') )
                {
                    // Kommentar
                    d_comments.append( d_line.mid( d_pos + 2 ).trimmed() );
                    d_pos = d_line.size();
                }
            }else
                break;
        }
    }
    Token token(TokenType tt, int len = 1, const QString &val = QString())
    {
        Token t( tt );
        t.d_name = val;
        t.d_lnr = d_lnr;
        d_pos += len;
        return t;
    }
    Token ident()
    {
        int off = 0;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() || ( !d_line[d_pos+off].isLetter() && d_line[d_pos+off] != QChar('_') ) )
                break;
            else
                off++;
        }
        return token( Ident, off, d_line.mid(d_pos, off ) );
    }
    Token string()
    {
        int off = 1;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() || d_line[d_pos+off] == QChar('\'') )
                break;
            else
                off++;
        }
        return token( Literal, off + 1, d_line.mid( d_pos + 1, off - 1 ) );
    }
    Token annotation()
    {
        int off = 1;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() || d_line[d_pos+off] == QChar(']') )
                break;
            else
                off++;
        }
        return token( Annotation, off + 1, d_line.mid( d_pos + 1, off - 1 ) );
    }
    void nextLine()
    {
        d_pos = 0;
        d_lnr++;
        d_line = d_in->readLine();
    }
private:
    QTextStream* d_in;
    QString d_line;
    quint16 d_pos;
    quint16 d_lnr;
    QStringList d_comments;
};

struct GeContext
{
    GeContext( QTextStream* in ):d_lex(in),d_curDef(0)
    {
        for( int i = Tok_always; i < Tok_maxKeyword; i++ )
        {
            d_terminals[ QByteArray(tokenToString(i)).toUpper() ] = i;
        }
    }
    GeLexer d_lex;
    GeLexer::Token d_curTok;
    Syntax::Definition* d_curDef;
    QString d_error;
    QHash<QString, int> d_terminals;
    void nextToken() { d_curTok = d_lex.nextToken(); }
};

// expression ::= term { "|" term }
// term ::= factor { factor }
// factor ::= ident | "(" expression ")*" | "(" expression ")?"

static Syntax::Node* parseExpressionG( GeContext& ctx );

static Syntax::Node* parseFactorG( GeContext& ctx )
{
    Syntax::Node* node = 0;

    switch( ctx.d_curTok.d_type )
    {
    case GeLexer::Ident:
        if( ctx.d_curTok.d_name[0].isUpper() )
        {
            int tok = ctx.d_terminals.value( ctx.d_curTok.d_name );
            if( tok == Tok_Invalid )
            {
                ctx.d_error = QString("invalid terminal '%1' on line %2").arg(ctx.d_curTok.d_name).arg(ctx.d_curTok.d_lnr);
                return 0;
            }else
            {
                node = new Syntax::Node( Syntax::Node::Terminal, ctx.d_curDef );
                node->d_lexTok = tok;
            }
        }else
        {
            node = new Syntax::Node( Syntax::Node::DefRef, ctx.d_curDef );
            node->d_name = ctx.d_curTok.d_name;
        }
        ctx.nextToken();
        break;
    case GeLexer::Literal:
        node = new Syntax::Node( Syntax::Node::Literal, ctx.d_curDef );
        node->d_name = ctx.d_curTok.d_name;
        ctx.nextToken();
        break;
    case GeLexer::LParen:
        {
            ctx.nextToken();
            QString annot;
            if( ctx.d_curTok.d_type == GeLexer::Annotation )
            {
                annot = ctx.d_curTok.d_name;
                ctx.nextToken();
            }
            node = parseExpressionG( ctx );
            if( node == 0 )
            {
                return 0;
            }
            if( ctx.d_curTok.d_type != GeLexer::RParen )
            {
                ctx.d_error = "expecting ')'";
                delete node;
                return 0;
            }
            ctx.nextToken();
            switch( ctx.d_curTok.d_type )
            {
            case GeLexer::Star:
                node->d_quant = Syntax::Node::ZeroOrMore;
                ctx.nextToken();
                break;
            case GeLexer::Question:
                node->d_quant = Syntax::Node::ZeroOrOne;
                ctx.nextToken();
                break;
            case GeLexer::Plus:
                node->d_quant = Syntax::Node::OneOrMore;
                ctx.nextToken();
                break;
            default:
                {
                    // Klammer dient nur zur Gruppierung
                }
                break;
            }
            if( node->d_type == Syntax::Node::Sequence || node->d_type == Syntax::Node::Alternative )
                node->d_name = annot;
        }
        break;
    default:
        ctx.d_error = "expecting ident, '()*' or '()?'";
        return 0;
    }
    return node;
}

static Syntax::Node* parseTermG( GeContext& ctx )
{
    Syntax::Node* node = 0;
    QString annot;
    if( ctx.d_curTok.d_type == GeLexer::Annotation )
    {
        annot = ctx.d_curTok.d_name;
        ctx.nextToken();
    }

    switch( ctx.d_curTok.d_type )
    {
    case GeLexer::Ident:
    case GeLexer::Literal:
    case GeLexer::LParen:
        node = parseFactorG( ctx );
        break;
    default:
        ctx.d_error = "expecting factor";
        return 0;
    }
    if( node == 0 )
        return 0;
    Syntax::Node* sequence = 0;
    while( ctx.d_curTok.d_type == GeLexer::Ident ||
           ctx.d_curTok.d_type == GeLexer::Literal ||
           ctx.d_curTok.d_type == GeLexer::LParen )
    {
        if( sequence == 0 )
        {
            sequence = new Syntax::Node( Syntax::Node::Sequence, ctx.d_curDef );
            sequence->d_subs.append( node );
            sequence->d_name = annot;
            node = sequence;
        }
        Syntax::Node* n = parseFactorG( ctx );
        if( n == 0 )
        {
            delete node;
            return 0;
        }
        sequence->d_subs.append(n);
    }
    return node;
}

static Syntax::Node* parseExpressionG( GeContext& ctx )
{
    Syntax::Node* node = 0;

    switch( ctx.d_curTok.d_type )
    {
    case GeLexer::Ident:
    case GeLexer::Literal:
    case GeLexer::LParen:
        node = parseTermG( ctx );
        break;
    default:
        ctx.d_error = "expecting term";
        return 0;
    }
    if( node == 0 )
        return 0;
    Syntax::Node* alternative = 0;
    while( ctx.d_curTok.d_type == GeLexer::Bar )
    {
        ctx.nextToken();
        if( alternative == 0 )
        {
            alternative = new Syntax::Node( Syntax::Node::Alternative, ctx.d_curDef );
            alternative->d_subs.append( node );
            node = alternative;
        }
        Syntax::Node* n = parseTermG( ctx );
        if( n == 0 )
        {
            delete node;
            return 0;
        }
        alternative->d_subs.append(n);
    }
    return node;
}

AntlrParser::AntlrParser(QObject* p):QObject(p),d_in(0)
{

}

bool AntlrParser::parse(QTextStream * in, Syntax* syn)
{
    Q_ASSERT( in != 0 && syn != 0 );
    d_in = in;
    d_syn = syn;

    d_lnr = 0;
    d_syn->initDefs(false);

    GeContext ctx(in);
    ctx.nextToken();
    while( ctx.d_curTok.d_type != GeLexer::End )
    {
        d_lnr = ctx.d_curTok.d_lnr;

        if( ctx.d_curTok.d_type != GeLexer::Ident )
            return error( "expecting category name" );
        if( d_syn->nodeFromDef( ctx.d_curTok.d_name ) != 0 )
            return error( "category already defined" );
        ctx.d_curDef = new Syntax::Definition();
        ctx.d_curDef->d_name = ctx.d_curTok.d_name;
        ctx.d_curDef->d_lnr = ctx.d_curTok.d_lnr;
        d_syn->addDef( ctx.d_curDef );

        ctx.nextToken();

        if( ctx.d_curTok.d_type == GeLexer::Exclam )
        {
            //ctx.d_curDef->d_keep = true;
            ctx.nextToken();
        }

        if( ctx.d_curTok.d_type != GeLexer::Colon )
            return error( "expecting colon" );

        ctx.nextToken();

        QString annot;
        if( ctx.d_curTok.d_type == GeLexer::Annotation )
        {
            annot = ctx.d_curTok.d_name;
            ctx.nextToken();
        }

        ctx.d_curDef->d_node = parseExpressionG(ctx);
        if( ctx.d_curTok.d_type == GeLexer::Invalid )
            return error( ctx.d_curTok.d_name.toUtf8() );
        else if( !ctx.d_error.isEmpty() )
            return error( ctx.d_error.toUtf8() );
        if( ctx.d_curTok.d_type != GeLexer::Semicolon )
            return error( "expecting semicolon" );

        if( ctx.d_curDef->d_node->d_type == Syntax::Node::Sequence ||
            ctx.d_curDef->d_node->d_type == Syntax::Node::Alternative )
            ctx.d_curDef->d_node->d_name = annot;

        ctx.nextToken();
    }
    d_comments = ctx.d_lex.getComments();
    return d_syn->checkRefs();
}

bool AntlrParser::error(const char* str)
{
    d_error = tr("Error (%1): %2").arg( d_lnr ).arg( str );
    return false;
}
