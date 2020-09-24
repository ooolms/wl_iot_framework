#ifndef ARPCEEPROMTEST_H
#define ARPCEEPROMTEST_H

#include <QtUnitTestSet.h>

class ARpcEEPROMTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcEEPROMTest(QObject *parent=nullptr);
	void testReadWriteConfig();
};

#endif // ARPCEEPROMTEST_H
