#include "ARpcSimpleAPITests.h"
#include "ARpcSimpleAPI/ARpcSimpleMsgDispatch.h"
#include "ARpcSimpleAPI/ARpcSyncUnsafeCall.h"
#include "ARpcSimpleAPI/ARpcSyncCall.h"
#include <QDebug>

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
	device=new ARpcTtyDevice("/dev/ttyACM0",cfg);
	return device->isConnected();
}

void ARpcSimpleAPITests::cleanup()
{
	delete device;
}

void ARpcSimpleAPITests::testOk()
{
	ARpcSyncCall call(cfg);
	QStringList rVal;
	VERIFY(call.call(device,ARpcMessage("testOk"),rVal))
}

void ARpcSimpleAPITests::testErr()
{
	ARpcSyncCall call(cfg);
	QStringList rVal;
	VERIFY(!call.call(device,ARpcMessage("testErr"),rVal))
	COMPARE(rVal.count(),1)
	COMPARE(rVal[0],QString::fromUtf8("epic fail с русским текстом"))
}

void ARpcSimpleAPITests::testLongCommand()
{
	ARpcSyncCall call(cfg);
	QStringList rVal;
	VERIFY(call.call(device,ARpcMessage("testLongCmd"),rVal))
}

void ARpcSimpleAPITests::testLongCommandNoSync()
{
	ARpcSyncCall call(cfg);
	ARpcSyncUnsafeCall call2(cfg);
	QStringList rVal;
	VERIFY(!call.call(device,ARpcMessage("testLongCmdNoSync"),rVal))
	VERIFY(call2.call(device,ARpcMessage("testLongCmdNoSync"),rVal))
}

void ARpcSimpleAPITests::testSimpleMsgDispatch()
{
	ARpcSimpleMsgDispatch disp(ARpcConfig(),device);
	QString infoMsg,measSens,measVal;
	connect(&disp,&ARpcSimpleMsgDispatch::infoMsg,[&infoMsg](const QString &str)
	{
		qDebug()<<"onInfoMsg";
		infoMsg=str;
	});
	connect(&disp,&ARpcSimpleMsgDispatch::measurementMsg,
		[&measSens,&measVal](const QString &sensor,const QString &value)
		{
			qDebug()<<"onMeasMsg";
			measSens=sensor;
			measVal=value;
		}
	);
	ARpcSyncUnsafeCall call(cfg);
	QStringList rVal;
	VERIFY(call.call(device,ARpcMessage("testInfoMsg"),rVal))
	COMPARE(infoMsg,QString("info_msg"))
	VERIFY(call.call(device,ARpcMessage("testMeasMsg"),rVal))
	COMPARE(measSens,QString("sens1"))
	COMPARE(measVal,QString("val1"))
}
