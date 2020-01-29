#ifndef ARPCDEVICESTATETEST_H
#define ARPCDEVICESTATETEST_H

#include "QtUnitTestSet.h"
#include "ARpcIWriteCallback.h"
#include "ARpcDeviceState.h"
#include "ARpcRealDeviceMessageDispatch.h"

class ARpcDeviceStateTestWriteCallback
	:public ARpcIWriteCallback
{
public:
	virtual void writeData(const char *data,unsigned long sz)override
	{
		buffer.append(data,sz);
	}

	virtual void writeStr(const char *str)override
	{
		buffer.append(str);
	}

public:
	QByteArray buffer;
};

class ARpcDeviceStateTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDeviceStateTest(QObject *parent=nullptr);
	virtual bool testInit();
	virtual void testCleanup();
	void testDump();
	void testNotifyCommandParamChanged();
	void testNotifyAdditionalParamChanged();
	void testNumConversion();

private:
	ARpcUuid devId;
	ARpcDeviceStateTestWriteCallback writeCb;
	ARpcStreamWriter writer;
	ARpcRealDeviceMessageDispatch disp;
	ARpcDeviceState *state;
};

#endif // ARPCDEVICESTATETEST_H
