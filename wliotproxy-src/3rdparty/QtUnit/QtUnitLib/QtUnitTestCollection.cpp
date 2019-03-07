#include "QtUnitTestCollection.h"

using namespace QtUnit;

QtUnit::QtUnitTestCollection::QtUnitTestCollection(QObject *parent)
	:QObject(parent)
{
	QtUnitTestCollection *parentColl=
		qobject_cast<QtUnitTestCollection*>(parent);
	if(parentColl!=0)parentColl->addTestCollection(this);
	wasProcessed=false;
}

QtUnit::QtUnitTestCollection::QtUnitTestCollection(const QString &name,QObject *parent)
	:QObject(parent)
{
	QtUnitTestCollection *parentColl=
		qobject_cast<QtUnitTestCollection*>(parent);
	if(parentColl!=0)parentColl->addTestCollection(this);
	collectionName=name;
}

QtUnit::QtUnitTestCollection::~QtUnitTestCollection()
{
}

void QtUnit::QtUnitTestCollection::addTestCollection(QtUnitTestCollection *collection)
{
	childCollections.append(collection);
}

void QtUnit::QtUnitTestCollection::addTestSet(QtUnitTestSet *set)
{
	childSets.append(set);
}

void QtUnit::QtUnitTestCollection::runTests(const QStringList &disabledColls,const QStringList &disabledSets)
{
	emit collectionBegin();
	for(int i=0;i<childCollections.count();++i)
	{
		if(!disabledColls.contains(childCollections[i]->collectionName))
			childCollections[i]->runTests(disabledColls,disabledSets);
		else childCollections[i]->skipCollection();
	}
	for(int i=0;i<childSets.count();++i)
	{
		if(!disabledSets.contains(childSets[i]->testSetName))childSets[i]->runTests();
		else childSets[i]->skipTestSet();
	}
	emit collectionEnd();
}

const QList<QtUnitTestCollection*>& QtUnit::QtUnitTestCollection::collections()
{
	return childCollections;
}

const QList<QtUnitTestSet*>& QtUnit::QtUnitTestCollection::sets()
{
	return childSets;
}

bool QtUnit::QtUnitTestCollection::hasFails()
{
	for(int i=0;i<childCollections.count();++i)
		if(childCollections[i]->hasFails())return true;
	for(int i=0;i<childSets.count();++i)
		if(childSets[i]->hasTestFails())return true;
	return false;
}

bool QtUnitTestCollection::processed()
{
	return wasProcessed;
}

void QtUnitTestCollection::skipCollection()
{
	for(int i=0;i<childCollections.count();++i)
		childCollections[i]->skipCollection();
	for(int i=0;i<childSets.count();++i)
		childSets[i]->skipTestSet();
	wasProcessed=false;
	emit collectionSkipped();
}
