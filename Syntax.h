#ifndef EBNF_SYNTAX_H
#define EBNF_SYNTAX_H

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
#include <QHash>
#include <QStringList>
#include <QSet>

class QTextStream;

namespace Vl
{
	// Syntax-File einlesen und Datenstruktur geeignet für Auswertungen zwecks Parserbau

	class Syntax : public QObject
	{
	public:
		struct Node;

		struct Definition
		{
            QString d_name;
            QString d_text;
            QString d_label;
            quint16 d_lnr;
			bool d_recursive;
			bool d_recPath; // liegt auf rekursivem Pfad, aber selber nicht unbedingt rekursiv
            qint8 d_level;
            bool d_visited;
            bool d_derefed;
            Node* d_node; // 0 für Primitives
			QSet<Node*> d_usedBy;
            Definition(const QString& name = QString() ):d_lnr(0),d_node(0),d_name(name),
                d_recursive(false),d_level(-1),d_visited(false),d_recPath(false),
                d_derefed(false){}
            ~Definition()
            {
                if( d_node ) delete d_node;
            }
			void dump() const;
		};
		typedef QHash<QString,Definition*> Definitions;
        typedef QSet<Syntax::Node*> NodeSet;
        typedef QList<NodeSet> LlkNodes;

		struct Node
		{
            enum Type { Terminal, DefRef, Literal, Sequence, Alternative };
			enum Quantity { One, ZeroOrOne, ZeroOrMore, OneOrMore };
			quint8 d_type;
			quint8 d_quant;
            quint8 d_lexTok;
			bool d_recursive;
            QString d_name; // auch Seq und Alt können optional einen Namen haben
            QByteArray d_attr; // predikate wie LL1, LL2, etc.
			QList<Node*> d_subs;
            NodeSet d_first;
			Definition* d_owner;
            Node(quint8 t, Definition* d, quint8 lt = 0):d_type(t),d_lexTok(lt),d_quant(0),d_owner(d),d_recursive(false){}
            Node(quint8 t, Definition* d, const QString& name):d_type(t),d_lexTok(0),d_quant(0),d_owner(d),d_recursive(false),d_name(name){}
            ~Node();
			void dump(int level = 0) const;
			Node* clone(Definition* newOwner) const;
            NodeSet calcFirstSet() const;
            LlkNodes calcLlkSet( int k ) const;
		};

		explicit Syntax(QObject *parent = 0);
		~Syntax();

        void initDefs(bool presetPrimitives);
        bool checkRefs();
        void clearDefs();

        bool addDef( Definition* ); // transfer ownership
        const Definitions& getDefs() const { return d_defs; }
        const Definition* getDef( const QString& name ) const;
        const Node* getRootNode( const QString& name ) const;
        const QList<Definition*>& getDefsInOrder() const { return d_order; }
		const Node* nodeFromDef( const QString& );
        const QStringList& getComments() const { return d_comments; }

        const QString& getError() const { return d_error; }

        void dump() const;
	protected:
		bool checkRefs( Node *node );
		void uncheckRefs( Definition* def );
		void uncheckRefs( Node* node );
        static NodeSet calcFirstSet(QList<const Node*>& stack );
	private:
        Q_DISABLE_COPY(Syntax)
		friend class SyntaxModifier;
        friend class SyntaxAnalyzer;
        QString d_error;
        Definitions d_defs;
        QList<Definition*> d_order;
        QStringList d_comments;
    };
}

#endif // EBNF_SYNTAX_H
