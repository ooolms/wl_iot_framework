#ifndef QTUNITWIDGET_H
#define QTUNITWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include "QtUnitTestCollection.h"

class QPushButton;

namespace QtUnit
{
	class QtUnitWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit QtUnitWidget(const QStringList &disabledColls,const QStringList &disabledSets,
			QtUnitTestCollection *coll,QWidget *parent=0);

	private slots:
		void runAllTests();
		void runSelectedTests();
		void onCollectionBegin();
		void onCollectionEnd();
		void onCollectionSkipped();
		void onSetBegin();
		void onSetEnd();
		void onSetSkipped();
		void onTestBegin(int testId);
		void onTestEnd(int testId,bool result,const QString &message,const QString &log);
		void onItemDClicked(QTreeWidgetItem *item);
		void onCopyTitleTriggered();
		void onCopyMessageTriggered();

	private:
		void setupCollection(QTreeWidgetItem *item,QtUnitTestCollection *col);
		void setupSet(QTreeWidgetItem *item,QtUnitTestSet *set);
		void resetTree();

	private:
		QtUnitTestCollection *collection;
		int nextId;
		QTreeWidget *tree;
		QPushButton *runAllBtn,*runSelectedBtn;
		QAction *copyTitleAction,*copyMessageAction;
		QIcon redTest,greenTest,grayTest,waitTest;
		QIcon redSet,greenSet,graySet,blueSet,waitSet;
		QMap<QtUnitTestCollection*,QTreeWidgetItem*> collectionsMap;
		QMap<QTreeWidgetItem*,QtUnitTestCollection*> collectionsReverseMap;
		QMap<QtUnitTestSet*,QTreeWidgetItem*> setsMap;
		QMap<QTreeWidgetItem*,QtUnitTestSet*> setsReverseMap;
		QMap<QtUnitTestSet*,QMap<int,QTreeWidgetItem*> > setsTestsMap;
		QStringList dColls,dSets;
	};
}

#endif // QTUNITWIDGET_H
