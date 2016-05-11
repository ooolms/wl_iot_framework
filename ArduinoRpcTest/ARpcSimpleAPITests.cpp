#include "ARpcSimpleAPITests.h"
#include "ARpcSimpleAPI/ARpcSimpleMsgDispatch.h"

ARpcSimpleAPITests::ARpcSimpleAPITests(QObject *parent)
	:QtUnitTestSet("ARpcComplexTests",parent)
{
	addTest((TestFunction)&ARpcSimpleAPITests::testOk,"Test ok func");
	addTest((TestFunction)&ARpcSimpleAPITests::testErr,"Test failing func");
	addTest((TestFunction)&ARpcSimpleAPITests::testLongCommand,"Test regular sync");
	addTest((TestFunction)&ARpcSimpleAPITests::testLongCommandNoSync,"Test long func with no sync");
	addTest((TestFunction)&ARpcSimpleAPITests::testSimpleMsgDispatch,"Test simple message dispatcher");
}

bool ARpcSimpleAPITests::init()
{
	ARpcConfig cfg;
	device=new ARpcTtyDevice("/dev/ttyACM0",cfg);
	return device->isConnected();
}

void ARpcSimpleAPITests::cleanup()
{
	delete device;
}

void ARpcSimpleAPITests::testOk()
{
	QStringList rVal;
	VERIFY(device->callSync(ARpcMessage("testOk"),rVal))
}

void ARpcSimpleAPITests::testErr()
{
	QStringList rVal;
	VERIFY(!device->callSync(ARpcMessage("testErr"),rVal))
	COMPARE(rVal.count(),1)
	COMPARE(rVal[0],QString::fromUtf8("epic fail с русским текстом"))
}

void ARpcSimpleAPITests::testLongCommand()
{
	QStringList rVal;
	VERIFY(device->callSync(ARpcMessage("testLongCmd"),rVal))
}

void ARpcSimpleAPITests::testLongCommandNoSync()
{
	QStringList rVal;
	VERIFY(!device->callSync(ARpcMessage("testLongCmdNoSync"),rVal))
	VERIFY(device->callSyncUnsafe(ARpcMessage("testLongCmdNoSync"),rVal))
}

void ARpcSimpleAPITests::testSimpleMsgDispatch()
{
	ARpcSimpleMsgDispatch disp(ARpcConfig(),device);
	QString infoMsg,measSens,measVal;
	connect(&disp,&ARpcSimpleMsgDispatch::infoMsg,[&infoMsg](const QString &str)
	{
		infoMsg=str;
	});
	connect(&disp,&ARpcSimpleMsgDispatch::measurementMsg,
		[&measSens,&measVal](const QString &sensor,const QString &value)
		{
			measSens=sensor;
			measVal=value;
		}
	);
	QStringList rVal;
	VERIFY(device->callSyncUnsafe(ARpcMessage("testInfoMsg"),rVal))
	COMPARE(infoMsg,QString("info_msg"))
	VERIFY(device->callSyncUnsafe(ARpcMessage("testMeasMsg"),rVal))
	COMPARE(measSens,QString("sens1"))
	COMPARE(measVal,QString("val1"))
}
