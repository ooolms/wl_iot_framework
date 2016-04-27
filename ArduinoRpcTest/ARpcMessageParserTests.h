#ifndef ARPCMESSAGEPARSERTESTS_H
#define ARPCMESSAGEPARSERTESTS_H

#include "QtUnitTestSet.h"
#include <QObject>

class ARpcMessageParserTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcMessageParserTests(QObject *parent=0);
	void testParseAndDump();
};

#endif // ARPCMESSAGEPARSERTESTS_H
