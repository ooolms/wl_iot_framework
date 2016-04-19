#include "ARpcComplexTests.h"

ARpcComplexTests::ARpcComplexTests(QObject *parent)
	:QtUnitTestSet("ARpcComplexTests",parent)
{
	addTest((TestFunction)&ARpcComplexTests::testOk,"Test ok func");
	addTest((TestFunction)&ARpcComplexTests::testErr,"Test failing func");
	addTest((TestFunction)&ARpcComplexTests::testLongCommand,"Test regular sync");
	addTest((TestFunction)&ARpcComplexTests::testLongCommandNoSync,"Test long func with no sync");
}

bool ARpcComplexTests::init()
{
	ARpcConfig cfg;
	device=new ARpcTtyDevice("/dev/ttyACM0",cfg);
	return device->isConnected();
}

void ARpcComplexTests::cleanup()
{
	delete device;
}

void ARpcComplexTests::testOk()
{
	QStringList rVal;
	VERIFY(device->callSync(ARpcMessage("testOk"),rVal))
}

void ARpcComplexTests::testErr()
{
	QStringList rVal;
	VERIFY(!device->callSync(ARpcMessage("testErr"),rVal))
	COMPARE(rVal.count(),1)
	COMPARE(rVal[0],QString("epic fail с русским текстом"))
}

void ARpcComplexTests::testLongCommand()
{
	QStringList rVal;
	VERIFY(device->callSync(ARpcMessage("testLongCmd"),rVal))
}

void ARpcComplexTests::testLongCommandNoSync()
{
	QStringList rVal;
	VERIFY(!device->callSync(ARpcMessage("testLongCmdNoSync"),rVal))
	VERIFY(device->callSyncUnsafe(ARpcMessage("testLongCmdNoSync"),rVal))
}
