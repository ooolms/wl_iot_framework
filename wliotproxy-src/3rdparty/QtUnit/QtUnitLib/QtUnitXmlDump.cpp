#include "QtUnitXmlDump.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

QtUnit::QtUnitXmlDump::QtUnitXmlDump(QObject *parent)
	:QObject(parent)
{
}

void QtUnit::QtUnitXmlDump::dump(QtUnit::QtUnitTestCollection *coll,const QString &fileName,
	const QString &project,const QString &author)
{
	QDomDocument doc;
	QDomElement rootNode=doc.createElement("tests_result");
	doc.appendChild(rootNode);
	rootNode.setAttribute("project",project);
	rootNode.setAttribute("author",author);
	dumpCollectionRecursive(coll,doc,rootNode);
	QFile file(fileName);
	if(file.open(QIODevice::WriteOnly))
	{
		file.write(doc.toByteArray());
		file.close();
	}
}

void QtUnit::QtUnitXmlDump::dumpCollectionRecursive(
	QtUnit::QtUnitTestCollection *coll,QDomDocument &doc,QDomElement &parent)
{
	QDomElement collElem=doc.createElement("collection");
	parent.appendChild(collElem);
	collElem.setAttribute("name",coll->collectionName);
	QString result;
	if(!coll->processed())result="skipped";
	else if(coll->hasFails())result="failed";
	else result="ok";
	collElem.setAttribute("result",result);
	for(QtUnitTestCollection *i:coll->collections())
		dumpCollectionRecursive(i,doc,collElem);
	for(QtUnitTestSet *s:coll->sets())
	{
		QDomElement setElem=doc.createElement("set");
		collElem.appendChild(setElem);
		setElem.setAttribute("name",s->testSetName);
		if(!s->processed())result="skipped";
		else if(s->hasInitFails())result="init_failed";
		else if(s->hasTestFails())result="failed";
		else result="ok";
		setElem.setAttribute("result",result);
		for(const QtUnitTestSet::TestDescr &t:s->tests())
		{
			QDomElement testElem=doc.createElement("test");
			setElem.appendChild(testElem);
			testElem.setAttribute("name",t.name);
			testElem.setAttribute("result",t.result?"ok":"failed");
			testElem.setAttribute("message",t.message);
			if(!t.log.isEmpty())
			{
				QDomElement logElem=doc.createElement("log");
				testElem.appendChild(logElem);
				logElem.appendChild(doc.createTextNode(t.log));
			}
		}
	}
}
