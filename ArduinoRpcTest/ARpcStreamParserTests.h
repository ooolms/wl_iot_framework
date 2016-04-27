#ifndef ARPCSTREAMPARSERTESTS_H
#define ARPCSTREAMPARSERTESTS_H

#include "QtUnitTestSet.h"

class ARpcStreamParserTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcStreamParserTests(QObject *parent=0);
	void testParsing();
};

#endif // ARPCSTREAMPARSERTESTS_H
