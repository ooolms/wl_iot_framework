#include "QtUnitTestSet.h"
#include "QtUnitTestCollection.h"
#include <exception>
#include <QApplication>

using namespace QtUnit;

int QtUnitTestSet::nextId=0;

QtUnit::QtUnitTestSet::QtUnitTestSet(QObject *parent)
	:QObject(parent)
{
	QtUnitTestCollection *parentColl=qobject_cast<QtUnitTestCollection*>(parent);
	if(parentColl!=nullptr)parentColl->addTestSet(this);
	testSetName="QtUnitTestSet";
	nextId=0;
	failsWhileRunning=false;
	wasProcessed=false;
}

QtUnit::QtUnitTestSet::QtUnitTestSet(const QString &name,QObject *parent)
	:QObject(parent)
{
	QtUnitTestCollection *parentColl=qobject_cast<QtUnitTestCollection*>(parent);
	if(parentColl!=nullptr)parentColl->addTestSet(this);
	testSetName=name;
	nextId=0;
	failsWhileRunning=false;
	initFails=false;
	wasProcessed=false;
}

QtUnit::QtUnitTestSet::~QtUnitTestSet()
{
}

int QtUnit::QtUnitTestSet::addTest(TestFunction func,const QString &testName)
{
	TestDescr descr;
	descr.id=nextId++;
	descr.name=testName;
	descr.func=func;
	testsList.append(descr);
	return descr.id;
}

void QtUnit::QtUnitTestSet::runTests()
{
	failsWhileRunning=false;
	initFails=false;
	wasProcessed=true;
	testLog.clear();
	emit testSetBegin();
	if(!init())
	{
		initFails=true;
		emit testSetEnd();
		return;
	}
	for(int i=0;i<testsList.count();++i)
		runTest(i);
	cleanup();
	emit testSetEnd();
}

bool QtUnit::QtUnitTestSet::init()
{
	return true;
}

void QtUnit::QtUnitTestSet::cleanup()
{
}

bool QtUnit::QtUnitTestSet::testInit()
{
	return true;
}

void QtUnit::QtUnitTestSet::testCleanup()
{
}

void QtUnit::QtUnitTestSet::writeLogMessage(const QString &html)
{
	testLog.append(html);
	testLog.append("<br/>\n");
}

const QList<QtUnitTestSet::TestDescr>& QtUnit::QtUnitTestSet::tests()
{
	return testsList;
}

const QtUnitTestSet::TestDescr* QtUnitTestSet::test(int id)
{
	for(int i=0;i<testsList.count();++i)
		if(testsList[i].id==id)return &testsList[i];
	return nullptr;
}

bool QtUnit::QtUnitTestSet::hasTestFails()
{
	return failsWhileRunning;
}

bool QtUnit::QtUnitTestSet::hasInitFails()
{
	return initFails;
}

bool QtUnit::QtUnitTestSet::processed()
{
	return wasProcessed;
}

void QtUnit::QtUnitTestSet::skipTestSet()
{
	wasProcessed=false;
	emit testSetSkipped();
}

void QtUnitTestSet::runSingleTest(int index)
{
	if(index<0||index>=testsList.count())return;
	failsWhileRunning=false;
	initFails=false;
	wasProcessed=true;
	testLog.clear();
	emit testSetBegin();
	if(!init())
	{
		initFails=true;
		emit testSetEnd();
		return;
	}
	runTest(index);
	cleanup();
	emit testSetEnd();
}

void QtUnitTestSet::runTest(int i)
{
	emit testBegin(testsList[i].id);
	result=true;
	summaryMessage="Ok";
	testLog.clear();
	TestFunction f=testsList[i].func;
	if(!testInit())
	{
		failsWhileRunning=true;
		testsList[i].result=false;
		testsList[i].message="test init fails: "+summaryMessage;
		testsList[i].log=testLog;
		emit testEnd(testsList[i].id,false,testsList[i].message,testsList[i].log);
		return;
	}
	try
	{
		(this->*f)();
	}
	catch(std::exception *ex)
	{
		summaryMessage=QString("exception: ")+ex->what();
		result=false;
	}
	catch(...)
	{
		summaryMessage="unknown exception";
		result=false;
	}
	testCleanup();
	testsList[i].result=result;
	testsList[i].message=summaryMessage;
	testsList[i].log=testLog;
	emit testEnd(testsList[i].id,result,summaryMessage,testLog);
	if(!result)failsWhileRunning=true;
	qApp->processEvents();
}
