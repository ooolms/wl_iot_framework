#ifndef ARPCUUIDTEST_H
#define ARPCUUIDTEST_H

#include "QtUnitTestSet.h"
#include <QUuid>

class ARpcUuidTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcUuidTest(QObject *parent=nullptr);
	void testIsValid();
	void testParseStr();
	void testParseHex();

private:
	QUuid id;
};

#endif // ARPCUUIDTEST_H
