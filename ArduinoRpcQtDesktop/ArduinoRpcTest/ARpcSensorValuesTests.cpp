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

#include "ARpcSensorValuesTests.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

static const QByteArrayList singleValue=QByteArrayList()<<"12.0"<<"54.3"<<"135.1";
static const QByteArrayList singleValueWithTime=QByteArrayList()<<"15391"<<"12.0"<<"54.3"<<"135.1";
static const float packetValue[]={12.0,54.3,135.1,20.5,56.1,63.6};
static const QVector<double> packetValue2={12.0,54.3,135.1,20.5,56.1,63.6};
static const QByteArrayList textValue=QByteArrayList()<<"text1";
static const QByteArrayList textValue2=QByteArrayList()<<"1234"<<"text2";

ARpcSensorValuesTests::ARpcSensorValuesTests(QObject *parent)
	:QtUnitTestSet("ARpcSensorValuesTest",parent)
{
	addTest((TestFunction)&ARpcSensorValuesTests::testSingleValue,"Test parsing single measurement");
	addTest((TestFunction)&ARpcSensorValuesTests::testPacketValue,"Test parsing packet of measurements");
	addTest((TestFunction)&ARpcSensorValuesTests::testTextValue,"Test parsing text");
}

void ARpcSensorValuesTests::testSingleValue()
{
	ARpcSingleSensorValue valNoTime(3);
	VERIFY(valNoTime.type()==ARpcSensorDef::SINGLE);
	VERIFY(valNoTime.parseMsgArgs(singleValue));
	COMPARE(valNoTime.values(),QVector<double>()<<12.0<<54.3<<135.1);
	ARpcSingleSensorValue valLocalTime(3,true);
	VERIFY(valLocalTime.type()==ARpcSensorDef::SINGLE_LT);
	VERIFY(valLocalTime.parseMsgArgs(singleValueWithTime));
	COMPARE(valLocalTime.values(),QVector<double>()<<12.0<<54.3<<135.1);
	COMPARE(valLocalTime.time(),15391);
	ARpcSingleSensorValue valGlobalTime(3,false);
	VERIFY(valGlobalTime.type()==ARpcSensorDef::SINGLE_GT);
	VERIFY(valGlobalTime.parseMsgArgs(singleValueWithTime));
	COMPARE(valGlobalTime.values(),QVector<double>()<<12.0<<54.3<<135.1);
	COMPARE(valGlobalTime.time(),15391);
}

void ARpcSensorValuesTests::testTextValue()
{
	ARpcTextSensorValue val;
	qint64 dt=QDateTime::currentMSecsSinceEpoch();
	VERIFY(val.parseMsgArgs(textValue));
	COMPARE(val.value(),"text1");
	VERIFY(val.time()>=dt)
	VERIFY(val.parseMsgArgs(textValue2));
	COMPARE(val.value(),"text2");
	VERIFY(val.time()==1234)
}

void ARpcSensorValuesTests::testPacketValue()
{
	QByteArrayList m;
	m.append(QByteArray((const char*)packetValue,sizeof(packetValue)).toBase64());
	ARpcPacketSensorValue valNoTime(3);
	VERIFY(valNoTime.type()==ARpcSensorDef::PACKET);
	VERIFY(valNoTime.parseMsgArgs(m));
	VERIFY(valNoTime.valuesCount()==2);
	for(int i=0;i<6;++i)
	{
		VERIFY(qAbs(valNoTime.values()[i]-packetValue2[i])<0.1)
	}
	m.insert(0,"15391");
	ARpcPacketSensorValue valLocalTime(3,true);
	VERIFY(valLocalTime.type()==ARpcSensorDef::PACKET_LT);
	VERIFY(valLocalTime.parseMsgArgs(m));
	VERIFY(valLocalTime.valuesCount()==2);
	for(int i=0;i<6;++i)
	{
		VERIFY(qAbs(valLocalTime.values()[i]-packetValue2[i])<0.1)
	}
	COMPARE(valLocalTime.time(),15391);
	ARpcPacketSensorValue valGlobalTime(3,false);
	VERIFY(valGlobalTime.type()==ARpcSensorDef::PACKET_GT);
	VERIFY(valGlobalTime.parseMsgArgs(m));
	VERIFY(valGlobalTime.valuesCount()==2);
	for(int i=0;i<6;++i)
	{
		VERIFY(qAbs(valLocalTime.values()[i]-packetValue2[i])<0.1)
	}
	COMPARE(valGlobalTime.time(),15391);
}

