#ifndef ARPCSIMPLEAPITESTS_H
#define ARPCSIMPLEAPITESTS_H

#include "QtUnitTestSet.h"
#include "ARpcDevices/ARpcTtyDevice.h"

//need TestSketch2 on Ardunio on /dev/ttyACM0
class ARpcSimpleAPITests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcSimpleAPITests(QObject *parent=0);
	virtual bool init();
	virtual void cleanup();
	void testOk();
	void testErr();
	void testLongCommand();
	void testLongCommandNoSync();
	void testSimpleMsgDispatch();

private:
	ARpcConfig cfg;
	ARpcTtyDevice *device;
};

#endif // ARPCSIMPLEAPITESTS_H
