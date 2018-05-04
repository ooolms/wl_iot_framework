#include "ARpcDeviceStateTest.h"

static const char *devIdStr="7f17a1de6e0946beb3630c9d1c98ceac";
static const char *devName="test";

ARpcDeviceStateTest::ARpcDeviceStateTest(QObject *parent)
	:QtUnitTestSet("ARpcDeviceStateTest",parent)
	,devId(devIdStr)
	,writer(&writeCb)
	,disp(devId,devName,&writer)
{
	addTest((TestFunction)&ARpcDeviceStateTest::testDump,"test dump state");
	addTest((TestFunction)&ARpcDeviceStateTest::testNotifyCommandParamChanged,"test notify on command param changed");
	addTest((TestFunction)&ARpcDeviceStateTest::testNotifyAdditionalParamChanged,
		"test notify on additional param changed");
}

bool ARpcDeviceStateTest::testInit()
{
	state=new ARpcDeviceState(&disp);
	state->prepareCommands(2);
	state->prepareCommand(0,"test1",1);
	state->prepareCommand(1,"test2",3);
	state->prepareAdditionalParameters(1);
	state->prepareAdditionalParameter(0,"zzz");
	writeCb.buffer.clear();
	return true;
}

void ARpcDeviceStateTest::testCleanup()
{
	delete state;
}

void ARpcDeviceStateTest::testDump()
{
	writer.beginWriteMsg();
	writer.writeArgNoEscape("ok");
	state->dump();
	writer.endWriteMsg();
	COMPARE(writeCb.buffer,QByteArray("ok|test1|1||test2|1||test2|2||test2|3||#|zzz|\n"))
}

void ARpcDeviceStateTest::testNotifyCommandParamChanged()
{
	state->setCommandParamState(1,2,"x");
	COMPARE(writeCb.buffer,QByteArray("statechanged|test2|3|x\n"))
}

void ARpcDeviceStateTest::testNotifyAdditionalParamChanged()
{
	state->setAdditionalParamState(0,"z");
	COMPARE(writeCb.buffer,QByteArray("statechanged|#|zzz|z\n"))
}
