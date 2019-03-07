#include "QtUnitConsoleRunner.h"

QtUnit::QtUnitConsoleRunner::QtUnitConsoleRunner(const QStringList &disabledColls,const QStringList &disabledSets,
	QtUnitTestCollection *coll,QObject *parent)
	:QObject(parent)
	,stdoutDbg(&stdoutFile)
{
	collection=coll;
	dColls=disabledColls;
	dSets=disabledSets;
	stdoutFile.open(2,QIODevice::WriteOnly);
	stdoutDbg=stdoutDbg.nospace();
	setupCollection(collection);
}

void QtUnit::QtUnitConsoleRunner::runTests()
{
	collection->runTests(dColls,dSets);
}

bool QtUnit::QtUnitConsoleRunner::hasFails()
{
	return collection->hasFails();
}

void QtUnit::QtUnitConsoleRunner::setupCollection(QtUnit::QtUnitTestCollection *col)
{
	connect(col,&QtUnitTestCollection::collectionBegin,this,&QtUnitConsoleRunner::onCollectionBegin);
	connect(col,&QtUnitTestCollection::collectionEnd,this,&QtUnitConsoleRunner::onCollectionEnd);
	connect(col,&QtUnitTestCollection::collectionSkipped,this,&QtUnitConsoleRunner::onCollectionSkipped);
	for(int i=0;i<col->collections().count();++i)
		setupCollection(col->collections()[i]);
	for(int i=0;i<col->sets().count();++i)
		setupSet(col->sets()[i]);
}

void QtUnit::QtUnitConsoleRunner::setupSet(QtUnit::QtUnitTestSet *set)
{
	connect(set,&QtUnitTestSet::testSetBegin,this,&QtUnitConsoleRunner::onSetBegin);
	connect(set,&QtUnitTestSet::testSetEnd,this,&QtUnitConsoleRunner::onSetEnd);
	connect(set,&QtUnitTestSet::testSetSkipped,this,&QtUnitConsoleRunner::onSetSkipped);
	connect(set,&QtUnitTestSet::testBegin,this,&QtUnitConsoleRunner::onTestBegin);
	connect(set,&QtUnitTestSet::testEnd,this,&QtUnitConsoleRunner::onTestEnd);
}

void QtUnit::QtUnitConsoleRunner::onCollectionBegin()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	stdoutDbg<<"RUN COLLECTION: "<<coll->collectionName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onCollectionEnd()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	if(coll->hasFails())
		stdoutDbg<<"COLLECTION FAILED: "<<coll->collectionName<<"\n";
	else stdoutDbg<<"COLLECTION PASSED: "<<coll->collectionName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onCollectionSkipped()
{
	QtUnitTestCollection *coll=qobject_cast<QtUnitTestCollection*>(sender());
	if(coll==0)return;
	stdoutDbg<<"COLLECTION SKIPPED: "<<coll->collectionName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onSetBegin()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	stdoutDbg<<"RUN SET: "<<set->testSetName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onSetEnd()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	if(set->hasInitFails())
		stdoutDbg<<"SET INIT FAILED: "<<set->testSetName<<"\n";
	else if(set->hasTestFails())
		stdoutDbg<<"SET FAILED: "<<set->testSetName<<"\n";
	else stdoutDbg<<"SET PASSED: "<<set->testSetName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onSetSkipped()
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	stdoutDbg<<"SET SKIPPED: "<<set->testSetName<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onTestBegin(int testId)
{
	Q_UNUSED(testId)
	//no output on begin
//	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
//	if(set==0)return;
//	const QtUnitTestSet::TestDescr *d=set->test(testId);
//	if(!d)return;
//	stdoutDbg<<"\tRUN TEST: "<<d->name<<"\n";
}

void QtUnit::QtUnitConsoleRunner::onTestEnd(int testId,bool result,const QString &message,const QString &log)
{
	QtUnitTestSet *set=qobject_cast<QtUnitTestSet*>(sender());
	if(set==0)return;
	const QtUnitTestSet::TestDescr *d=set->test(testId);
	if(!d)return;
	if(!result)stdoutDbg<<"\tTEST FAILED: ";
	else stdoutDbg<<"\tTEST PASSED: ";
	stdoutDbg<<d->name<<": "<<message<<"\n";
	if(!result&&!log.isEmpty())
		stdoutDbg<<"*********LOG********\n"<<log<<"\n*****************\n";
}
