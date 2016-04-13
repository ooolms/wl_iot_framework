#ifndef ARPCMESSAGETESTS_H
#define ARPCMESSAGETESTS_H

#include "qtunittestset.h"

class ARpcMessageTests
	:public QtUnit::QtUnitTestSet
{
public:
	explicit ARpcMessageTests(QObject *parent=0);
	void testParseAndDump();
};

#endif // ARPCMESSAGETESTS_H
