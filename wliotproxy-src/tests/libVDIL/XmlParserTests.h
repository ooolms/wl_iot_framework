#ifndef XMLPARSERTESTS_H
#define XMLPARSERTESTS_H

#include "QtUnitTestSet.h"

class XmlParserTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit XmlParserTests(QObject *parent=nullptr);
	void mainTest();
};

#endif // XMLPARSERTESTS_H
