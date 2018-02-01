/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcSimpleAPITests.h"
#include "ARpcBase/ARpcSimpleMsgDispatch.h"
#include "ARpcBase/ARpcUnsafeCall.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QDebug>

ARpcSimpleAPITests::ARpcSimpleAPITests(QObject *parent)
	:QtUnitTestSet("ARpcSimpleAPITests",parent)
{
	addTest((TestFunction)&ARpcSimpleAPITests::testOk,"Test ok func");
	addTest((TestFunction)&ARpcSimpleAPITests::testErr,"Test failing func");
	addTest((TestFunction)&ARpcSimpleAPITests::testLongCommand,"Test regular sync");
	addTest((TestFunction)&ARpcSimpleAPITests::testLongCommandNoSync,"Test long func with no sync");
	addTest((TestFunction)&ARpcSimpleAPITests::testSimpleMsgDispatch,"Test simple message dispatcher");
}

bool ARpcSimpleAPITests::init()
{
	device=new ARpcTtyDevice("/dev/ttyACM0");
	return device->isConnected();
}

void ARpcSimpleAPITests::cleanup()
{
	delete device;
}

void ARpcSimpleAPITests::testOk()
{
	ARpcSyncCall call;
	QByteArrayList rVal;
	VERIFY(call.call(device,"testOk",rVal));
}

void ARpcSimpleAPITests::testErr()
{
	ARpcSyncCall call;
	QByteArrayList rVal;
	VERIFY(!call.call(device,"testErr",rVal));
	COMPARE(rVal.count(),1);
	COMPARE(rVal[0],QString::fromUtf8("epic fail с русским текстом"));
}

void ARpcSimpleAPITests::testLongCommand()
{
	ARpcSyncCall call;
	QByteArrayList rVal;
	VERIFY(call.call(device,"testLongCmd",rVal));
}

void ARpcSimpleAPITests::testLongCommandNoSync()
{
	ARpcSyncCall call;
	ARpcUnsafeCall call2;
	QByteArrayList rVal;
	VERIFY(!call.call(device,"testLongCmdNoSync",rVal));
	VERIFY(call2.call(device,"testLongCmdNoSync",rVal));
}

void ARpcSimpleAPITests::testSimpleMsgDispatch()
{
	ARpcSimpleMsgDispatch disp(device);
	QByteArray infoMsg,measSens,measVal;
	connect(&disp,&ARpcSimpleMsgDispatch::infoMsg,[&infoMsg](const QByteArray &str)
	{
		qDebug()<<"onInfoMsg";
		infoMsg=str;
	});
	connect(&disp,&ARpcSimpleMsgDispatch::measurementMsg,
		[&measSens,&measVal](const QByteArray &sensor,const QByteArray &value)
		{
			qDebug()<<"onMeasMsg";
			measSens=sensor;
			measVal=value;
		}
	);
	ARpcUnsafeCall call;
	QByteArrayList rVal;
	VERIFY(call.call(device,"testInfoMsg",rVal));
	COMPARE(infoMsg,QString("info_msg"));
	VERIFY(call.call(device,"testMeasMsg",rVal));
	COMPARE(measSens,QString("sens1"));
	COMPARE(measVal,QString("val1"));
}
