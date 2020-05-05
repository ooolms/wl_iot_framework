#include "QtUnitWidget.h"
#include "ShowLogDialog.h"
#include <QPushButton>
#include <QLayout>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QClipboard>

static inline void initrc()
{
	Q_INIT_RESOURCE(qtunit);
}

using namespace QtUnit;
static const int collectionItemType=1;
static const int setItemType=2;
static const int testItemType=3;

QtUnit::QtUnitWidget::QtUnitWidget(const QStringList &disabledColls,const QStringList &disabledSets,
	QtUnitTestCollection *coll,QWidget *parent)
	:QWidget(parent)
{
	initrc();
	nextId=0;
	collection=coll;
	dColls=disabledColls;
	dSets=disabledSets;
	redTest=QIcon(":/qtunit/red_test.png");
	greenTest=QIcon(":/qtunit/green_test.png");
	grayTest=QIcon(":/qtunit/gray_test.png");
	waitTest=QIcon(":/qtunit/wait_test.png");

	redSet=QIcon(":/qtunit/red_set.png");
	greenSet=QIcon(":/qtunit/green_set.png");
	graySet=QIcon(":/qtunit/gray_set.png");
	waitSet=QIcon(":/qtunit/wait_set.png");
	blueSet=QIcon(":/qtunit/blue_set.png");

	tree=new QTreeWidget(this);
	tree->setHeaderLabels(QStringList()<<"title"<<"message");
	tree->setColumnWidth(0,300);
	tree->setIconSize(QSize(16,16));
	tree->setContextMenuPolicy(Qt::ActionsContextMenu);
	copyTitleAction=new QAction("Copy title",tree);
	copyMessageAction=new QAction("Copy message",tree);
	tree->addAction(copyTitleAction);
	tree->addAction(copyMessageAction);

	QWidget *toolbar=new QWidget(this);
	runAllBtn=new QPushButton("Run all",toolbar);
	runSelectedBtn=new QPushButton("Run selected",toolbar);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(toolbar);
	mainLayout->addWidget(tree,1);

	QHBoxLayout *toolbarLayout=new QHBoxLayout(toolbar);
	toolbarLayout->addWidget(runAllBtn);
	toolbarLayout->addWidget(runSelectedBtn);
	toolbarLayout->addStretch(1);

	connect(runAllBtn,&QPushButton::clicked,this,&QtUnitWidget::runAllTests);
	connect(runSelectedBtn,&QPushButton::clicked,this,&QtUnitWidget::runSelectedTests);
	connect(tree,&QTreeWidget::itemDoubleClicked,this,&QtUnitWidget::onItemDClicked);
	connect(copyTitleAction,&QAction::triggered,this,&QtUnitWidget::onCopyTitleTriggered);
	connect(copyMessageAction,&QAction::triggered,this,&QtUnitWidget::onCopyMessageTriggered);

	QTreeWidgetItem *item=new QTreeWidgetItem(tree);
	setupCollection(item,collection);
}

void QtUnit::QtUnitWidget::setupCollection(QTreeWidgetItem *item,QtUnitTestCollection *col)
{
	collectionsMap[col]=item;
	collectionsReverseMap[item]=col;
	connect(col,&QtUnitTestCollection::collectionBegin,this,&QtUnitWidget::onCollectionBegin);
	connect(col,&QtUnitTestCollection::collectionEnd,this,&QtUnitWidget::onCollectionEnd);
	connect(col,&QtUnitTestCollection::collectionSkipped,this,&QtUnitWidget::onCollectionSkipped);
	item->setText(0,col->collectionName);
	item->setIcon(0,graySet);
	item->setData(0,Qt::UserRole,collectionItemType);
	for(int i=0;i<col->collections().count();++i)
	{
		QTreeWidgetItem *child=new QTreeWidgetItem(item);
		setupCollection(child,col->collections()[i]);
	}
	for(int i=0;i<col->sets().count();++i)
	{
		QTreeWidgetItem *child=new QTreeWidgetItem(item);
		setupSet(child,col->sets()[i]);
	}
	item->setExpanded(true);
}

void QtUnit::QtUnitWidget::setupSet(QTreeWidgetItem *item,QtUnitTestSet *set)
{
	setsMap[set]=item;
	setsReverseMap[item]=set;
	connect(set,&QtUnitTestSet::testSetBegin,this,&QtUnitWidget::onSetBegin);
	connect(set,&QtUnitTestSet::testSetEnd,this,&QtUnitWidget::onSetEnd);
	connect(set,&QtUnitTestSet::testSetSkipped,this,&QtUnitWidget::onSetSkipped);
	connect(set,&QtUnitTestSet::testBegin,this,&QtUnitWidget::onTestBegin);
	connect(set,&QtUnitTestSet::testEnd,this,&QtUnitWidget::onTestEnd);
	QMap<int,QTreeWidgetItem*> testsMap;
	item->setText(0,set->testSetName);
	item->setIcon(0,graySet);
	item->setData(0,Qt::UserRole,setItemType);
	for(int i=0;i<set->tests().count();++i)
	{
		QTreeWidgetItem *child=new QTreeWidgetItem(item);
		child->setText(0,set->tests()[i].name);
		child->setIcon(0,grayTest);
		child->setData(0,Qt::UserRole,testItemType);
		testsMap[set->tests()[i].id]=child;
	}
	setsTestsMap[set]=testsMap;
}

void QtUnit::QtUnitWidget::resetTree()
{
	tree->collapseAll();
	for(auto item:collectionsMap)
	{
		item->setIcon(0,graySet);
		item->setText(1,"");
		item->setExpanded(true);
	}
	for(auto item:setsMap)
	{
		item->setIcon(0,graySet);
		item->setText(1,"");
	}
	for(auto map:setsTestsMap)
	{
		for(auto item:map)
		{
			item->setIcon(0,grayTest);
			item->setData(0,Qt::UserRole+1,QString());
			item->setText(1,"");
		}
	}
}

void QtUnit::QtUnitWidget::runAllTests()
{
	runAllBtn->setEnabled(false);
	runSelectedBtn->setEnabled(false);
	resetTree();
	if(collection)collection->runTests(dColls,dSets);
	runAllBtn->setEnabled(true);
	runSelectedBtn->setEnabled(true);
}

void QtUnit::QtUnitWidget::runSelectedTests()
{
	if(tree->selectedItems().count()==0)return;
	QTreeWidgetItem *item=tree->selectedItems()[0];
	int type=item->data(0,Qt::UserRole).toInt();
	runAllBtn->setEnabled(false);
	runSelectedBtn->setEnabled(false);
	resetTree();
	if(type==collectionItemType&&collectionsReverseMap.contains(item))
		collectionsReverseMap[item]->runTests(dColls,dSets);
	else if(type==setItemType&&setsReverseMap.contains(item))
		setsReverseMap[item]->runTests();
	else if(type==testItemType&&setsReverseMap.contains(item->parent()))
		setsReverseMap[item->parent()]->runSingleTest(item->parent()->indexOfChild(item));
	runAllBtn->setEnabled(true);
	runSelectedBtn->setEnabled(true);
}

void QtUnit::QtUnitWidget::onCollectionBegin()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	QTreeWidgetItem *item=collectionsMap[coll];
	if(item)item->setIcon(0,waitSet);
}

void QtUnit::QtUnitWidget::onCollectionEnd()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	QTreeWidgetItem *item=collectionsMap[coll];
	if(item)
	{
		if(coll->hasFails())
		{
			item->setIcon(0,redSet);
			item->setText(1,"Failed");
			item->setExpanded(true);
		}
		else
		{
			item->setIcon(0,greenSet);
			item->setText(1,"Ok");
		}
	}
}

void QtUnit::QtUnitWidget::onCollectionSkipped()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	QTreeWidgetItem *item=collectionsMap[coll];
	if(item)
	{
		item->setIcon(0,blueSet);
		item->setText(1,"Skipped");
	}
}

void QtUnit::QtUnitWidget::onSetBegin()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	QTreeWidgetItem *item=setsMap[set];
	if(item)item->setIcon(0,waitSet);
}

void QtUnit::QtUnitWidget::onSetEnd()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	QTreeWidgetItem *item=setsMap[set];
	if(item)
	{
		if(set->hasInitFails())
		{
			item->setIcon(0,redSet);
			item->setText(1,"Test set init failed");
		}
		else if(set->hasTestFails())
		{
			item->setIcon(0,redSet);
			item->setText(1,"Failed");
		}
		else
		{
			item->setIcon(0,greenSet);
			item->setText(1,"Ok");
		}
	}
}

void QtUnit::QtUnitWidget::onSetSkipped()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	QTreeWidgetItem *item=setsMap[set];
	if(item)
	{
		item->setIcon(0,blueSet);
		item->setText(1,"Skipped");
	}
}

void QtUnit::QtUnitWidget::onTestBegin(int testId)
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	QTreeWidgetItem *item=setsTestsMap[set][testId];
	if(item)item->setIcon(0,waitTest);
}

void QtUnit::QtUnitWidget::onTestEnd(int testId,bool result,const QString &message,const QString &log)
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	QTreeWidgetItem *item=setsTestsMap[set][testId];
	if(item)
	{
		if(!result)item->setIcon(0,redTest);
		else item->setIcon(0,greenTest);
		item->setText(1,message);
		item->setToolTip(1,message);
		item->setData(0,Qt::UserRole+1,log);
	}
}

void QtUnit::QtUnitWidget::onItemDClicked(QTreeWidgetItem *item)
{
	int type=item->data(0,Qt::UserRole).toInt();
	if(type!=testItemType)return;
	QString log=item->data(0,Qt::UserRole+1).toString();
	if(log.isEmpty())return;
	ShowLogDialog dlg(log,this);
	dlg.showMaximized();
	dlg.exec();
}

void QtUnitWidget::onCopyTitleTriggered()
{
	if(tree->selectedItems().isEmpty())return;
	qApp->clipboard()->setText(tree->selectedItems()[0]->text(0));
}

void QtUnitWidget::onCopyMessageTriggered()
{
	if(tree->selectedItems().isEmpty())return;
	qApp->clipboard()->setText(tree->selectedItems()[0]->text(1));
}
