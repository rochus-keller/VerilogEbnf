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
#include "Syntax.h"
#include "SyntaxModifier.h"
#include "SyntaxAnalyzer.h"
#include "CocoGen.h"
#include "ParserGenerator.h"
#include "EbnfParser.h"
#include "AntlrParser.h"
#include "BisonGen.h"
#include <Vhdl/Tokens.h>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QShortcut>
#include <QHeaderView>
#include <Gui2/AutoMenu.h>
#include <QFileDialog>
#include <QApplication>
using namespace Vl;

enum Columns { _Text, _Level, _UsedBy, _First };

static inline QString _format( const QList<Syntax::Definition*>& defs )
{
    QStringList str;
    foreach( const Syntax::Definition* def, defs )
    {
        str.append( def->d_name );
    }
    str.sort();
    return str.join(QChar('\n'));
}

static inline QString _format( const QSet<Syntax::Node*>& nodes )
{
    QList<Syntax::Definition*> defs;
    foreach( Syntax::Node* node, nodes )
    {
        Q_ASSERT( node->d_owner != 0 );
        if( !defs.contains(node->d_owner) )
            defs.append(node->d_owner);
    }
    return _format( defs );
}

static inline QString _quant( quint8 q, const QString& txt )
{
	switch( q )
	{
	case Syntax::Node::One:
		return txt;
	case Syntax::Node::ZeroOrOne:
		return QString("[ %1 ]").arg(txt);
		break;
	case Syntax::Node::ZeroOrMore:
		return QString("{ %1 }").arg(txt);
	case Syntax::Node::OneOrMore:
		return QString("{ %1 }+").arg(txt);
	default:
		return "<unknown quantifier>";
	}
}

static inline QString annotation( const QString& str )
{
	if( str.isEmpty() )
		return QString();
	else
		return QString("  (%1)").arg(str);
}

static inline QString _join( const QSet<QString>& ss )
{
    QStringList str;
    foreach( const QString& s, ss )
        str << s;
    str.sort();
    return str.join(' ');
}

class NodeTreeItem : public QTreeWidgetItem
{
public:
	Syntax::Node* d_node;
    Syntax::Definition* d_def;
	Syntax* d_syn;
    QSet<QString> d_first;
	NodeTreeItem( QTreeWidgetItem* p, Syntax* syn, Syntax::Node* node ):
        QTreeWidgetItem(p, node->d_type ),d_node(node),d_syn(syn),d_def(0)
        { if( d_node->d_type == Syntax::Node::DefRef )
            d_def = d_syn->getDefs().value(d_node->d_name); }
	QVariant data ( int column, int role ) const
	{
		Q_ASSERT( d_node != 0 );
		switch( role )
		{
		case Qt::DisplayRole:
			switch( column )
			{
			case _Text:
				switch( d_node->d_type )
				{
				case Syntax::Node::Terminal:
                    return _quant( d_node->d_quant, tokenToString( d_node->d_lexTok ) );
				case Syntax::Node::Literal:
					return _quant( d_node->d_quant, QString("\"%1\"").arg( d_node->d_name ) );
                case Syntax::Node::DefRef:
					return _quant( d_node->d_quant, d_node->d_name );
				case Syntax::Node::Sequence:
					return _quant( d_node->d_quant, "seq" ) + annotation(d_node->d_name);
				case Syntax::Node::Alternative:
					return _quant( d_node->d_quant, "alt" ) + annotation(d_node->d_name);
				default:
					break;
				}
            case _Level:
                if( d_def && d_def->d_level >= 0 )
                    return QString::number( d_def->d_level );
                break;
            case _UsedBy:
                if( d_def )
                    return QString::number( d_def->d_usedBy.size() );
                break;
            case _First:
                return _join( d_first );
			default:
				break;
			}
			break;
		case Qt::FontRole:
			if( column == _Text )
				switch( d_node->d_type )
				{
				case Syntax::Node::Terminal:
					{
						QFont f = treeWidget()->font();
						f.setBold(true);
						return f;
					}
					break;
                case Syntax::Node::DefRef:
					{
						QFont f = treeWidget()->font();
						f.setItalic(true);
						return f;
					}
					break;
				}
			break;
		case Qt::ForegroundRole:
			switch( column )
			{
			case _Text:
				switch( d_node->d_type )
				{
				case Syntax::Node::Sequence:
				case Syntax::Node::Alternative:
					return QBrush( Qt::blue );
				default:
					break;
				}
			default:
				break;
			}
			break;
		case Qt::BackgroundColorRole:
            if( column == _Level && d_node->d_type == Syntax::Node::DefRef && d_def && d_def->d_recPath )
                return QColor(Qt::yellow);
            else if( column == _Text && d_def && d_def->d_recursive )
                return QColor(Qt::yellow);
            break;
		case Qt::ToolTipRole:
			switch( column )
			{
			case _Text:
                if( d_def )
                    return ParserGenerator::format( *d_def );
                break;
            case _UsedBy:
                if( d_def )
                    return _format( d_def->d_usedBy );
                break;
            case _Level:
                return d_node->d_attr;
            default:
				break;
			}
			break;
		}
		return QTreeWidgetItem::data( column, role );
	}
};

class DefTreeItem : public QTreeWidgetItem
{
public:
    Syntax::Definition* d_def;
    Syntax* d_syn;
    QSet<QString> d_first;
    DefTreeItem( QTreeWidget* p, Syntax* syn, Syntax::Definition* def ):
        QTreeWidgetItem(p ),d_def(def),d_syn(syn){ }
    QVariant data ( int column, int role ) const
    {
        Q_ASSERT( d_def != 0 );
        switch( role )
        {
        case Qt::DisplayRole:
            switch( column )
            {
            case _Text:
                return d_def->d_name;
            case _Level:
                if( d_def->d_level >= 0 )
                    return QString::number( d_def->d_level );
                break;
            case _UsedBy:
                return QString::number( d_def->d_usedBy.size() );
            default:
                break;
            }
            break;
        case Qt::ToolTipRole:
            if( column == _UsedBy )
                return _format( d_def->d_usedBy );
            else if( column == _Text )
                return ParserGenerator::format( *d_def );
            break;
        case Qt::BackgroundColorRole:
            if( column == _Level && d_def->d_recPath )
                return QColor(Qt::yellow);
            else if( column == _Text && d_def->d_recursive )
                return QColor(Qt::yellow);
            break;
        }
        return QTreeWidgetItem::data( column, role );
    }
};

SyntaxTreeView::SyntaxTreeView(QWidget *parent) :
	QTreeWidget(parent)
{
	d_syn = new Syntax( this );
    d_ana = new SyntaxAnalyzer(d_syn);
	setAlternatingRowColors(true);
	setRootIsDecorated(true);
	setAllColumnsShowFocus(true);
	setHeaderLabels( QStringList() << tr("Item") << tr("Level") << tr("Used By") << tr("First") );
    // TODO header()->setResizeMode( _Text, QHeaderView::Stretch );

	Gui2::AutoMenu* pop = new Gui2::AutoMenu( this, true );
    pop->addCommand( tr("Follow"), this, SLOT(onFollow()), tr("CTRL+F"), true );
    pop->addCommand( tr("Expand selected"), this, SLOT(onExpandSel()), tr("CTRL+E"), true );
    pop->addCommand( tr("Expand all"), this, SLOT(onExpandAll()) );
    pop->addCommand( tr("Dereference"), this, SLOT(onDeref()), tr("CTRL+D"), true );
    pop->addCommand( tr("Sorting enabled"), this, SLOT(onEnableSort()) );
    pop->addSeparator();
    pop->addCommand( tr("Min. Levels && Recursions"), this, SLOT(onCalcPaths()) );
    pop->addCommand( tr("Max. Levels"), this, SLOT(onCalcMaxLevels()) );
    pop->addSeparator();
    pop->addCommand( tr("Open..."), this, SLOT(onOpen()), tr("CTRL+O"), true );
    pop->addCommand( tr("Save Coco"), this, SLOT(onSaveCoco()), tr("CTRL+S"), true );
    pop->addCommand( tr("Save ANTLR"), this, SLOT(onSaveAntlr()) );
    pop->addCommand( tr("Save Bison"), this, SLOT(onSaveBison()) );
    pop->addCommand( tr("Save PEG"), this, SLOT(onSavePeg()) );
    pop->addCommand( tr("Save SLK"), this, SLOT(onSaveSlk()) );
    pop->addCommand( tr("Save PCCTS"), this, SLOT(onSavePccts()) );
    pop->addCommand( tr("Reload"), this, SLOT(onReload()) );

	connect( this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onDoubleClicked(QTreeWidgetItem*,int)) );
    new QShortcut( tr("ALT+LEFT"), this, SLOT(onBack()) );

    new QShortcut( tr("CTRL+Q"), this, SLOT(close()) );
}

SyntaxTreeView::~SyntaxTreeView()
{
    delete d_ana;
}

static QSet<QString> _firstSet( Syntax::Node* node )
{
    const Syntax::NodeSet fs = node->calcFirstSet();
    QSet<QString> set;
    foreach( Syntax::Node* n, fs )
	{
        switch( n->d_type )
        {
        case Syntax::Node::Terminal:
            set.insert( tokenToString( n->d_lexTok ) );
            break;
        case Syntax::Node::DefRef:
            qWarning() << "Unresolved node" << n->d_name;
            set.insert( n->d_name );
            break;
        default:
            qWarning() << "Invalid node in first set" << n->d_type << n->d_name;
        }
    }
	return set;
}

static void markUpward( QTreeWidgetItem* item )
{
    if( item->backgroundColor( _First ) != Qt::red )
        item->setBackgroundColor( _First, Qt::yellow );
    if( item->parent() )
        markUpward( item->parent() );
}

static void loadNode( QTreeWidgetItem* parent, Syntax::Node* node, Syntax* syn )
{
	NodeTreeItem* item = new NodeTreeItem( parent, syn, node );
    item->d_first = _firstSet( node );
	foreach( Syntax::Node* n, node->d_subs )
        loadNode( item, n, syn );
	if( node->d_type == Syntax::Node::Alternative )
	{
		for( int i = 0; i < node->d_subs.size(); i++ )
		{
			NodeTreeItem* a = static_cast<NodeTreeItem*>(item->child(i));
			for( int j = 0; j < node->d_subs.size(); j++ )
			{
				NodeTreeItem* b = static_cast<NodeTreeItem*>(item->child(j));
				if( i != j && !( a->d_first & b->d_first ).isEmpty() )
				{
					item->child(i)->setBackgroundColor( _First, Qt::red );
					item->child(j)->setBackgroundColor( _First, Qt::red );
                    markUpward( item );
				}
			}
		}
	}
}

bool SyntaxTreeView::loadFromFile(const QString &path)
{
	QFile f(path);
	if( !f.open(QIODevice::ReadOnly) )
		return false;
	bool res = true;
    clear();
    d_histo.clear();
    QString err;
	const QString suff = QFileInfo(path).suffix();
    if( suff == "syn" || suff == "txt" || suff == "ebnf" )
    {
        EbnfParser p;
        res = p.parse( &f, d_syn );
        err = p.getError();
    }else if( suff == "g" )
    {
        QTextStream in( &f );
        AntlrParser p;
        res = p.parse( &in, d_syn );
        err = p.getError();
    }
	if( !res )
	{
        QMessageBox::critical(this,tr("Reading Syntax File"), err );
		return false;
	}
	d_path = path;
	reload();
	return true;
}

void SyntaxTreeView::reload()
{
	if( !d_syn->getError().isEmpty() )
		return;

    SyntaxModifier a( d_syn );
    a.calcLevelAndRecursionPaths( d_syn->getDefs().value("source_text") );

    clear();
	d_histo.clear();

    QMap<QString,Syntax::Definition*> sorter;
    Syntax::Definitions::const_iterator i;
	for( i = d_syn->getDefs().begin(); i != d_syn->getDefs().end(); ++i )
	{
		sorter.insert( i.key(), i.value() );
	}
    d_ana->calcFirstSets();
    QMap<QString,Syntax::Definition*>::iterator j;
	for( j = sorter.begin(); j != sorter.end(); ++j )
	{
        Syntax::Definition* def = j.value();
        DefTreeItem* item = new DefTreeItem( this, d_syn, def );
//		if( def->d_level == 0 )
//			item->setBackgroundColor( _Level, Qt::blue );
		if( def->d_node )
        {
            loadNode( item, def->d_node, d_syn );
        }
	}
    //expandAll();
    resizeColumnToContents(0);
}

bool SyntaxTreeView::gotoCategory(const QString & str)
{
	QList<QTreeWidgetItem *> res = findItems( str, Qt::MatchFixedString, _Text );
	if( res.size() == 1 )
	{
		setCurrentItem( res.first(), _Text, QItemSelectionModel::ClearAndSelect );
		scrollToItem( res.first() );
		return true;
	}
	return false;
}

void SyntaxTreeView::onDoubleClicked(QTreeWidgetItem *item, int)
{
	NodeTreeItem* i = dynamic_cast<NodeTreeItem*>(item);
    if( i && i->d_node->d_type == Syntax::Node::DefRef )
	{
		if( gotoCategory( i->d_node->d_name ) )
			d_histo.push_back( item );
	}
}

void SyntaxTreeView::onBack()
{
	if( !d_histo.isEmpty() )
	{
		QTreeWidgetItem* item = d_histo.back();
		d_histo.pop_back();
		setCurrentItem( item, _Text, QItemSelectionModel::ClearAndSelect );
		scrollToItem( item );
	}
}

void SyntaxTreeView::onSaveAntlr()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    ParserGenerator pg( d_syn );
    pg.generateAntlr( info.dir().absoluteFilePath( info.baseName() + ".g"), true );
	pg.generateLevelList( info.dir().absoluteFilePath( info.baseName() + ".levels.csv") );
}

void SyntaxTreeView::onDenormalize()
{
	ENABLED_IF( currentItem() && currentItem()->type() == 0 ); // 0 sind Definitions

	const int index = indexOfTopLevelItem( currentItem() );
	const QString name = currentItem()->text(_Text);
	clear();
	SyntaxModifier a( d_syn );
	if( a.denormalizeDef( name ) )
	{
		reload();
		focusOnIndex(index);
	}
}

void SyntaxTreeView::onReload()
{
	ENABLED_IF(true);
    loadFromFile(d_path);
}

void SyntaxTreeView::onJoinOneOrMore()
{
	ENABLED_IF(true);
	const int index = indexOfTopLevelItem( currentItem() );
	clear();
	SyntaxModifier a( d_syn );
	a.joinOneOrMore();
	reload();
	focusOnIndex(index);
}

void SyntaxTreeView::onDenormAllSingles()
{
	ENABLED_IF(true);
	if( QMessageBox::warning(this,tr("Denormalize all singles"), tr("Shure?"),
							 QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
		return;
	const int index = indexOfTopLevelItem( currentItem() );
	clear();
	SyntaxModifier a( d_syn );
	a.denormalizeAllSingles();
	reload();
	focusOnIndex(index);
}

void SyntaxTreeView::onRemoveAliasses()
{
	ENABLED_IF(true);
	const int index = indexOfTopLevelItem( currentItem() );
	clear();
	SyntaxModifier a( d_syn );
	a.removeAliasses();
	reload();
	focusOnIndex(index);
}

void SyntaxTreeView::focusOnIndex(int index)
{
	if( index >= 0 && index < topLevelItemCount() )
	{
		setCurrentItem( topLevelItem(index), _Text, QItemSelectionModel::ClearAndSelect );
		scrollToItem( topLevelItem(index) );
	}
}

void SyntaxTreeView::onNormalizeAlts()
{
	ENABLED_IF(true);
	const int index = indexOfTopLevelItem( currentItem() );
	clear();
	SyntaxModifier a( d_syn );
	a.normalizeAlts();
	reload();
	focusOnIndex(index);
}

void SyntaxTreeView::onInline()
{
	NodeTreeItem* item = dynamic_cast<NodeTreeItem*>( currentItem() );
    ENABLED_IF( item && item->d_node->d_type == Syntax::Node::DefRef );

	const int index = indexOfTopLevelItem( item );
	clear();
	SyntaxModifier a( d_syn );
	a.inlineCatRef( item->d_node );
	reload();
	focusOnIndex(index);
}

void SyntaxTreeView::onCalcPaths()
{
    DefTreeItem* item = dynamic_cast<DefTreeItem*>( currentItem() );
    ENABLED_IF( item );
    SyntaxModifier a( d_syn );
    a.calcLevelAndRecursionPaths(item->d_def);
    //a.calcLevelAndLoops(true);
    update();
}

void SyntaxTreeView::onCalcMaxLevels()
{
    ENABLED_IF(true);
    SyntaxModifier m( d_syn );
    m.calcLevelAndLoops(false);
    update();
}

void SyntaxTreeView::onOpen()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    const QString path = QFileDialog::getOpenFileName( this, tr("Open File"), info.absoluteDir().absolutePath() );
    if( path.isEmpty() )
        return;

    loadFromFile(path);
}

void SyntaxTreeView::onExpandAll()
{
    ENABLED_IF(true);
    expandAll();
}

static void expandSel( QTreeWidgetItem* item )
{
    for( int i = 0; i < item->childCount(); i++ )
        expandSel( item->child(i) );
    item->setExpanded(true);
}

void SyntaxTreeView::onExpandSel()
{
    ENABLED_IF( currentItem() );

    expandSel( currentItem() );
}

void SyntaxTreeView::onDeref()
{
    NodeTreeItem* item = dynamic_cast<NodeTreeItem*>( currentItem() );
    ENABLED_IF( item && item->d_node->d_type == Syntax::Node::DefRef );

    Syntax::Definition* def = d_syn->getDefs().value(item->d_node->d_name);
    if( def == 0 )
        return; // kommt nicht vor

    loadNode( item, def->d_node, d_syn );
    expandSel( item );
}

void SyntaxTreeView::onFollow()
{
    NodeTreeItem* item = dynamic_cast<NodeTreeItem*>( currentItem() );
    ENABLED_IF( item && item->d_node->d_type == Syntax::Node::DefRef );

    if( gotoCategory( item->d_node->d_name ) )
        d_histo.push_back( item );
}

void SyntaxTreeView::onEnableSort()
{
    CHECKED_IF( true, isSortingEnabled() );
    setSortingEnabled( !isSortingEnabled() );
    if( !isSortingEnabled() )
        // funktioniert nicht: sortByColumn(-1);
        reload();
}

void SyntaxTreeView::onSaveCoco()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    CocoGen pg( d_syn );
    pg.generateCoco( info.dir().absoluteFilePath( info.baseName() + ".atg") );
}

void SyntaxTreeView::onSaveBison()
{
    ENABLED_IF(true);

    BisonGen gen( d_syn );
    QFileInfo info(d_path);
    gen.generate( info.dir().absoluteFilePath( info.baseName() + ".yy"), false );
}

void SyntaxTreeView::onSavePeg()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    ParserGenerator pg( d_syn );
    pg.generatePeg( info.dir().absoluteFilePath( info.baseName() + ".peg"), true );
}

void SyntaxTreeView::onSaveSlk()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    ParserGenerator pg( d_syn );
    pg.generateSlk( info.dir().absoluteFilePath( info.baseName() + ".slk"), true );
}

void SyntaxTreeView::onSavePccts()
{
    ENABLED_IF(true);

    QFileInfo info(d_path);
    ParserGenerator pg( d_syn );
    pg.generatePccts( info.dir().absoluteFilePath( info.baseName() + ".g"), true );
}
