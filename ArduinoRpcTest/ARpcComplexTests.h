#ifndef ARPCCOMPLEXTESTS_H
#define ARPCCOMPLEXTESTS_H

#include "qtunittestset.h"
#include "ARpcTtyDevice.h"

//need TestSketch2 on Ardunio on /dev/ttyACM0
class ARpcComplexTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcComplexTests(QObject *parent=0);
	virtual bool init();
	virtual void cleanup();
	void testOk();
	void testErr();
	void testLongCommand();
	void testLongCommandNoSync();

private:
	ARpcTtyDevice *device;
};

#endif // ARPCCOMPLEXTESTS_H
