#ifndef SYNTAXTREEVIEW_H
#define SYNTAXTREEVIEW_H

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

#include <QTreeWidget>

namespace Vl
{
	class Syntax;
    class SyntaxAnalyzer;

	class SyntaxTreeView : public QTreeWidget
	{
		Q_OBJECT
	public:
		explicit SyntaxTreeView(QWidget *parent = 0);
        ~SyntaxTreeView();

		bool loadFromFile( const QString& path );
		void reload();
		bool gotoCategory( const QString& );
	protected slots:
		void onDoubleClicked( QTreeWidgetItem * item, int column );
		void onBack();
        void onSaveAntlr();
		void onDenormalize();
		void onReload();
		void onJoinOneOrMore();
		void onDenormAllSingles();
		void onRemoveAliasses();
		void focusOnIndex(int);
		void onNormalizeAlts();
		void onInline();
		void onCalcPaths();
        void onCalcMaxLevels();
        void onOpen();
        void onExpandAll();
        void onExpandSel();
        void onDeref();
        void onFollow();
        void onEnableSort();
        void onSaveCoco();
        void onSaveBison();
        void onSavePeg();
        void onSaveSlk();
        void onSavePccts();
    private:
		Syntax* d_syn;
        SyntaxAnalyzer* d_ana;
		QList<QTreeWidgetItem*> d_histo;
		QString d_path;
	};
}

#endif // SYNTAXTREEVIEW_H
