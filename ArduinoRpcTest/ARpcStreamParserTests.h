#ifndef ARPCSTREAMPARSERTESTS_H
#define ARPCSTREAMPARSERTESTS_H

#include "qtunittestset.h"

class ARpcStreamParserTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcStreamParserTests(QObject *parent=0);
	void testParsing();
};

#endif // ARPCSTREAMPARSERTESTS_H
