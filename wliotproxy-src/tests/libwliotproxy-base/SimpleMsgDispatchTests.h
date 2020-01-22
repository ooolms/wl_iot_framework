#ifndef SIMPLEMSGDISPATCHTESTS_H
#define SIMPLEMSGDISPATCHTESTS_H

#include "QtUnitTestSet.h"
#include "FakeDevice.h"
#include "wliot/devices/SimpleMsgDispatch.h"

class SimpleMsgDispatchTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit SimpleMsgDispatchTests(QObject *parent=nullptr);
	virtual bool init();
	virtual void cleanup();
	virtual bool testInit();
	void testInfoMsgDispatch();
	void testMeasMsgDispatch();
	void testCommandStateChangeMsgDispatch();
	void testAdditionalStateChangeMsgDispatch();

private:
	FakeDeviceBackend *device;
	SimpleMsgDispatch *disp;
};

#endif // SIMPLEMSGDISPATCHTESTS_H
