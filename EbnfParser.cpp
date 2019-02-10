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
#include "Syntax.h"
#include <Verilog/VlToken.h>
#include <QTextStream>
#include <QtDebug>
using namespace Vl;

// Aus Ada::Syntax adaptiert

static const int s_para = 167; // §

class SynLexer
{
public:
    enum TT { Invalid, Terminal, DefRef, Bar, LPar, RPar, LBrack, RBrack, LBrace, RBrace, Attr, End };
    struct Token
    {
        quint8 d_type;
        quint8 d_adaToken;
        QByteArray d_name;
        Token(TT t = Invalid, quint8 tt = Tok_Invalid):d_type(t),d_adaToken(tt){}
        QByteArray print() const
        {
            switch( d_type )
            {
            case Invalid:
                return "?";
            case Terminal:
                return tokenToString(d_adaToken, d_name );
            case DefRef:
                return d_name;
            case Bar:
                return "|";
            case LPar:
                return "[!";
            case RPar:
                return "!]";
            case LBrack:
                return "[";
            case RBrack:
                return "]";
            case LBrace:
                return "{";
            case RBrace:
                return "}";
            case Attr:
                return "\\" + d_name + "\\";
            case End:
                return "<end>";
            }
            return QByteArray();
        }
    };
    Token nextToken()
    {
        while( d_pos < d_line.size() )
        {
            skipWhiteSpace();
            const int ch = quint8(d_line[d_pos]);
            switch( ch )
            {
            case '+':
                if( lookAhead(1) == ':' )
                    return token( Terminal, Tok_PlusColon, 2 );
                else
                    return token( Terminal, Tok_Plus );
            case '-':
                switch( lookAhead(1) )
                {
                case ':':
                    return token( Terminal, Tok_MinusColon, 2 );
                case '>':
                    return token( Terminal, Tok_MinusGt, 2 );
                default:
                    return token( Terminal, Tok_Minus );
                }
                break;
            case '!':
                if( lookAhead(1) == ']' )
                    return token( RPar, Tok_Invalid, 2 );
                else if( lookAhead(1) == '=' )
                {
                    if( lookAhead(2) == '=' )
                        return token( Terminal, Tok_Bang2Eq, 3 );
                    else
                        return token( Terminal, Tok_BangEq, 2 );
                }else
                    return token( Terminal, Tok_Bang );
                break;
            case '~':
                switch( lookAhead(1) )
                {
                case '|':
                    return token( Terminal, Tok_TildeBar,2);
                case '&':
                    return token( Terminal, Tok_TildeAmp,2);
                case '^':
                    return token( Terminal, Tok_TildeHat,2);
                default:
                    return token( Terminal, Tok_Tilde );
                }
                break;
            case '^':
                if( lookAhead(1) == '~' )
                    return token( Terminal, Tok_HatTilde, 2 );
                else
                    return token( Terminal, Tok_Hat );
            case '/':
                return token( Terminal, Tok_Slash );
            case '%':
                return token( Terminal, Tok_Percent );
            case '=':
                if( lookAhead(1) == '=' )
                {
                    if( lookAhead(2) == '=' )
                        return token( Terminal, Tok_3Eq, 3 );
                    else
                        return token( Terminal, Tok_2Eq, 2 );
                }else if( lookAhead(1) == '>' )
                    return token( Terminal, Tok_EqGt, 2 );
                else
                    return token( Terminal, Tok_Eq );
            case '&':
                if( lookAhead(1) == '&' )
                {
                    if( lookAhead(2) == '&' )
                        return token( Terminal, Tok_3Amp, 3 );
                    else
                        return token( Terminal, Tok_2Amp, 2 );
                }else
                    return token( Terminal, Tok_Amp );
            case '*':
                switch( lookAhead(1) )
                {
                case '*':
                    return token( Terminal, Tok_2Star, 2 );
                case '>':
                    return token( Terminal, Tok_StarGt, 2 );
                case ')':
                    return token( Terminal, Tok_Ratt, 2 );
                default:
                    return token( Terminal, Tok_Star );
                }
            case '<':
                switch( lookAhead(1) )
                {
                case '<':
                    if( lookAhead(2) == '<' )
                        return token( Terminal, Tok_3Lt, 3 );
                    else
                        return token( Terminal, Tok_2Lt, 2 );
                case '=':
                    return token( Terminal, Tok_Leq, 2 );
                default:
                    return token( Terminal, Tok_Lt );
                }
            case '>':
                switch( lookAhead(1) )
                {
                case '>':
                    if( lookAhead(2) == '>' )
                        return token( Terminal, Tok_3Gt, 3 );
                    else
                        return token( Terminal, Tok_2Gt, 2 );
                case '=':
                    return token( Terminal, Tok_Geq, 2 );
                default:
                    return token( Terminal, Tok_Gt );
                }
            case '(':
                if( lookAhead(1) == '*' )
                    return token( Terminal, Tok_Latt, 2 );
                else
                    return token( Terminal, Tok_Lpar );
            case ')':
                return token( Terminal, Tok_Rpar );

            case '[':
                if( lookAhead(1) == '!' )
                    return token( LPar, Tok_Invalid, 2 );
                else
                    return token( LBrack );
            case ']':
                return token( RBrack );
            case '{':
                if( lookAhead(1) == '*' )
                {
                    const int pos = d_line.indexOf( "*}", d_pos + 2 );
                    if( pos != -1 )
                    {
                        d_pos = pos + 2;
                        continue;
                    }else
                        return token( Invalid, Tok_Invalid, 2, QString("missing *}").toLatin1() );
                }else
                    return token( LBrace );
                break;
            case '}':
                return token( RBrace );
            case '|':
                return token( Bar );
            case ',':
                return token( Terminal, Tok_Comma );
            case '.':
                return token( Terminal, Tok_Dot );
            case ';':
                return token( Terminal, Tok_Semi );
            case '#':
                return token( Terminal, Tok_Hash );
            case '@':
                return token( Terminal, Tok_At );
            case '?':
                return token( Terminal, Tok_Qmark );
            case ':':
                return token( Terminal, Tok_Colon );
            }
            if( ch == '"' )
            {
                return string();
            }else if( ch == '\\' )
            {
                return attribute();
            }else if( ::isalnum(ch) || ch == '$' )
            {
                // Identifier oder Reserved Word
                return ident();
            }else
            {
                // Error
                return token( Invalid, Tok_Invalid, 1, QString("unexpected character '%1' %2").arg(char(ch)).arg(ch).toLatin1() );
            }
        }
        return Token(End);
    }
    void setSource( const QByteArray& line )
    {
        d_line = line;
        d_pos = 0;
        d_lastTokenType = Invalid;
    }
    SynLexer( const QByteArray& source = QByteArray() ):d_line(source),d_pos(0),d_lastTokenType(Invalid)
    {
        d_types.insert("LBR",Tok_Lbrack);
        d_types.insert("RBR",Tok_Rbrack);
        d_types.insert("LCU",Tok_Lbrace);
        d_types.insert("RCU",Tok_Rbrace);
    }
protected:
    void skipWhiteSpace()
    {
        while( d_pos < d_line.size() && ::isspace(d_line[d_pos]) )
            d_pos++;
    }
    Token token(TT tt, quint8 ttt = Tok_Invalid, int len = 1, const QByteArray &val = QByteArray())
    {
        Token t( tt, ttt );
        t.d_name = val;
        d_pos += len;
        d_lastTokenType = tt;
        return t;
    }
    int lookAhead(int off = 1) const
    {
        if( int( d_pos + off ) < d_line.size() )
        {
            const int res = quint8(d_line[ d_pos + off ]);
            return res;
        }else
            return 0;
    }
    Token ident()
    {
        int off = 0;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() ||
                    ( !::isalnum(d_line[d_pos+off]) && d_line[d_pos+off] != '_' && d_line[d_pos+off] != '$' ) )
                break;
            else
                off++;
        }
        const QByteArray str = d_line.mid(d_pos, off );
        TokenType tt = d_types.value(str,Tok_Invalid);
        if( tt != Tok_Invalid )
            return token( Terminal, tt, off );
        tt = matchReservedWord( str );
        if( tt != Tok_Invalid )
            return token( Terminal, tt, off );
        else if( str == "IDENT" )
            return token( Terminal, Tok_Ident, off );
        else if( str == "STRING" )
            return token( Terminal, Tok_String, off );
        else if( str == "SYSNAME" )
            return token( Terminal, Tok_SysName, off );
        else if( str == "REALNUM" )
            return token( Terminal, Tok_Realnum, off );
        else if( str == "NATURAL" )
            return token( Terminal, Tok_Natural, off );
        else if( str == "SIZEDBASED" )
            return token( Terminal, Tok_SizedBased, off );
        else if( str == "BASEDINT" )
            return token( Terminal, Tok_BasedInt, off );
        else if( str == "BASEFMT" )
            return token( Terminal, Tok_BaseFormat, off );
        else if( str == "BASEVAL" )
            return token( Terminal, Tok_BaseValue, off );
        else if( str == "CODI" )
            return token( Terminal, Tok_CoDi, off );
        else
            return token( DefRef, Tok_Ident, off, str );
    }
    Token attribute()
    {
        int off = 1;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() || d_line[d_pos+off] == '\\' )
                break;
            else
                off++;
        }
        const QByteArray str = d_line.mid(d_pos + 1, off - 1 );
        return token( Attr, Tok_Invalid, off+1, str );
    }
    Token string()
    {
        int off = 1;
        while( true )
        {
            if( (d_pos+off) >= d_line.size() || d_line[d_pos+off] == '"' )
                break;
            else
                off++;
        }
        const QByteArray str = d_line.mid(d_pos + 1, off - 1 );
        return token( Terminal, Tok_String, off+1, str );
    }

private:
    QByteArray d_line;
    quint16 d_pos;
    TT d_lastTokenType;
    QHash<QByteArray,TokenType> d_types;
};

struct SynContext
{
    SynLexer d_lex;
    SynLexer::Token d_curTok;
    Syntax::Definition* d_curDef;
    QByteArray d_error;
    QByteArray d_ref;
    void nextToken() { d_curTok = d_lex.nextToken(); }
    QByteArray getError() const
    {
        if( d_curTok.d_type == SynLexer::Invalid )
            return d_curTok.d_name;
        else if( !d_error.isEmpty() )
            return d_error + " at " + d_curTok.print();
        else if( d_curTok.d_type != SynLexer::End )
            return "unexpected symbols after expression";
        else
            return QByteArray();
    }
};

// expression ::= term { "|" term }
// term ::= factor { factor }
// factor ::= keyword | delimiter | category | "[" expression "]" | "{" expression "}"

static Syntax::Node* parseExpression( SynContext& ctx );

static bool checkCardinality( SynContext& ctx, Syntax::Node* node )
{
    if( node->d_quant != Syntax::Node::One )
    {
        ctx.d_error = "contradicting nested quantifiers";
        delete node;
        return false;
    }
    if( node->d_type != Syntax::Node::Sequence && node->d_type != Syntax::Node::Alternative )
    {
        Q_ASSERT( node->d_subs.isEmpty() );
        return true;
    }
    if( node->d_subs.isEmpty() )
    {
        ctx.d_error = "container with zero items";
        delete node;
        return false;
    }
    if( node->d_subs.size() == 1 &&
            ( node->d_subs.first()->d_type == Syntax::Node::Sequence
              || node->d_subs.first()->d_type == Syntax::Node::Alternative))
    {
        ctx.d_error = "container containing only one other sequence or alternative";
        delete node;
        return false;
    }
    return true;
}

static Syntax::Node* parseFactor( SynContext& ctx )
{
    Syntax::Node* node = 0;

    switch( ctx.d_curTok.d_type )
    {
    case SynLexer::Terminal:
        node = new Syntax::Node( Syntax::Node::Terminal, ctx.d_curDef );
        node->d_lexTok = ctx.d_curTok.d_adaToken;
        ctx.nextToken();
        break;
    case SynLexer::DefRef:
        node = new Syntax::Node( Syntax::Node::DefRef, ctx.d_curDef );
        node->d_name = ctx.d_curTok.d_name;
        ctx.nextToken();
        break;
    case SynLexer::LBrack:
        {
            ctx.nextToken();
            node = parseExpression( ctx );
            if( node == 0 )
            {
                return 0;
            }
            if( ctx.d_curTok.d_type != SynLexer::RBrack )
            {
                ctx.d_error = "expecting ']'";
                delete node;
                return 0;
            }
            if( !checkCardinality(ctx,node) )
                return 0;
            node->d_quant = Syntax::Node::ZeroOrOne;
            ctx.nextToken();
        }
        break;
    case SynLexer::LPar:
        {
            ctx.nextToken();
            node = parseExpression( ctx );
            if( node == 0 )
            {
                return 0;
            }
            if( ctx.d_curTok.d_type != SynLexer::RPar )
            {
                ctx.d_error = "expecting '!]'";
                delete node;
                return 0;
            }
            if( !checkCardinality(ctx,node) )
                return 0;
            node->d_quant = Syntax::Node::One;
            ctx.nextToken();
        }
        break;
    case SynLexer::LBrace:
        {
            ctx.nextToken();
            node = parseExpression( ctx );
            if( node == 0 )
            {
                return 0;
            }
            if( ctx.d_curTok.d_type != SynLexer::RBrace )
            {
                ctx.d_error = "expecting '}'";
                delete node;
                return 0;
            }
            if( !checkCardinality(ctx,node) )
                return 0;
            node->d_quant = Syntax::Node::ZeroOrMore;
            ctx.nextToken();
        }
        break;
    default:
        ctx.d_error = "expecting keyword, delimiter, category, '{' or '['";
        return 0;
    }
    return node;
}

static Syntax::Node* parseTerm( SynContext& ctx )
{
    Syntax::Node* node = 0;

    QByteArray attr;
    if( ctx.d_curTok.d_type == SynLexer::Attr )
    {
        attr = ctx.d_curTok.d_name;
        ctx.nextToken();
    }

    switch( ctx.d_curTok.d_type )
    {
    case SynLexer::Terminal:
    case SynLexer::DefRef:
    case SynLexer::LBrack:
    case SynLexer::LBrace:
    case SynLexer::LPar:
        node = parseFactor( ctx );
        break;
    default:
        ctx.d_error = "expecting factor";
        return 0;
    }
    if( node == 0 )
        return 0;

    Syntax::Node* sequence = 0;
    while( ctx.d_curTok.d_type == SynLexer::Terminal ||
           ctx.d_curTok.d_type == SynLexer::DefRef ||
           ctx.d_curTok.d_type == SynLexer::LBrack ||
           ctx.d_curTok.d_type == SynLexer::LPar ||
           ctx.d_curTok.d_type == SynLexer::LBrace )
    {
        if( sequence == 0 )
        {
            sequence = new Syntax::Node( Syntax::Node::Sequence, ctx.d_curDef );
            sequence->d_subs.append(node);
            node = sequence;
        }
        Syntax::Node* n = parseFactor( ctx );
        if( n == 0 )
        {
            delete node;
            return 0;
        }
        sequence->d_subs.append(n);
    }

    if( node )
        node->d_attr = attr; // node ist eine Sequence!
    return node;
}

static Syntax::Node* parseExpression( SynContext& ctx )
{
    Syntax::Node* node = 0;

    switch( ctx.d_curTok.d_type )
    {
    case SynLexer::Terminal:
    case SynLexer::DefRef:
    case SynLexer::LBrack:
    case SynLexer::LPar:
    case SynLexer::LBrace:
    case SynLexer::Attr:
        node = parseTerm( ctx );
        break;
    default:
        ctx.d_error = "expecting term";
        return 0;
    }
    if( node == 0 )
        return 0;
    Syntax::Node* alternative = 0;
    while( ctx.d_curTok.d_type == SynLexer::Bar )
    {
        ctx.nextToken();
        if( alternative == 0 )
        {
            alternative = new Syntax::Node( Syntax::Node::Alternative, ctx.d_curDef );
            alternative->d_subs.append(node);
            node = alternative;
        }
        Syntax::Node* n = parseTerm( ctx );
        if( n == 0 )
        {
            delete node;
            return 0;
        }
        alternative->d_subs.append(n);
    }
    return node;
}

static QByteArray parseDef(Syntax::Definition* def)
{
//    if( def->d_name == "library_declaration" )
//        qDebug() << "hit";
    SynContext ctx;
    ctx.d_curDef = def;
    ctx.d_lex.setSource( def->d_text.simplified().toLatin1() );
    ctx.nextToken();
    def->d_node = parseExpression(ctx);
    def->d_label = QString::fromLatin1(ctx.d_ref);
    return ctx.getError();
}

EbnfParser::EbnfParser(QObject* p):QObject(p),d_in(0)
{

}

bool EbnfParser::parse(QIODevice* in, Syntax* syn )
{
    Q_ASSERT( in != 0 && syn != 0 );
    d_in = in;
    d_syn = syn;

    d_lnr = 0;
    d_syn->initDefs(true);
    Syntax::Definition* curDef = 0;
    while( d_in && !d_in->atEnd() )
    {
        const QByteArray line = readLine();
        const QByteArray trimmedLine = line.trimmed();
        if( trimmedLine.isEmpty() )
            continue;
        if( trimmedLine.startsWith( "//" ) )
            continue;
        Q_ASSERT( !line.isEmpty() );
        if( !::isspace(line[0]) )
        {
            if( curDef != 0 )
            {
                const QByteArray err = parseDef( curDef );
                if( !err.isEmpty() )
                    return error(err,curDef->d_lnr);
                curDef = 0;
            }
            const int pos = line.indexOf( "::=" );
            if( pos == -1 )
                return error( "missing '::='", d_lnr );
            const QByteArray def = line.left(pos).trimmed();
            if( d_syn->nodeFromDef( def ) != 0 )
                return error( "category already defined", d_lnr );
            if( matchReservedWord( def ) != Tok_Invalid )
                return error( "category and keyword ambiguity", d_lnr );

            curDef = new Syntax::Definition();
            curDef->d_label = QString::fromLatin1( d_curLabel );
            curDef->d_name = QString::fromLatin1( def );
            curDef->d_text = QString::fromLatin1( line.mid( pos + 3 ) );
            curDef->d_lnr = d_lnr;
            if( !d_syn->addDef( curDef ) )
            {
                delete curDef;
                curDef = 0;
                return error( d_syn->getError().toUtf8(), d_lnr );
            }
        }else
        {
            if( !curDef->d_text.isEmpty() )
                curDef->d_text += "\n";
            curDef->d_text += line;
        }
    }
    if( curDef != 0 )
    {
        const QByteArray err = parseDef( curDef );
        if( !err.isEmpty() )
            return error(err, curDef->d_lnr);
        curDef = 0;
    }
    if( !d_syn->checkRefs() )
    {
        d_error = d_syn->getError();
        return false;
    }else
        return true;
}

QByteArray EbnfParser::readLine()
{
    d_lnr++;
    if( d_in )
        return d_in->readLine();
    else
        return QByteArray();
}

bool EbnfParser::error(const char* str, int lnr)
{
    d_error = tr("Error (%1): %2").arg( lnr ).arg( str );
    return false;
}

char EbnfParser::peekChar()
{
    Q_ASSERT( d_in != 0 );
    if( d_in->atEnd() )
        return 0;
    const int pos = d_in->pos();
    const char ch = d_in->read(1)[0];
    d_in->seek(pos);
    return ch;
}


void* AttrParser::parse(const QByteArray& line)
{
    SynContext ctx;
    ctx.d_curDef = 0;
    ctx.d_lex.setSource( line );
    ctx.nextToken();
    Syntax::Node* node = parseExpression(ctx);
    d_error = ctx.getError();
    return node;
}
