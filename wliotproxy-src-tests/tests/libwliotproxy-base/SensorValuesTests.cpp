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

#include "SensorValuesTests.h"
#include "wliot/devices/SensorValue.h"
#include "TestData.h"
#include <QDateTime>
#include <QVector>

using namespace WLIOT;

SensorValuesTests::SensorValuesTests(QObject *parent)
	:QtUnitTestSet("SensorValuesTest",parent)
{
	addTest((TestFunction)&SensorValuesTests::testSingleValue,"Test parsing single measurement");
	addTest((TestFunction)&SensorValuesTests::testPacketValue,"Test parsing packet of measurements");
	addTest((TestFunction)&SensorValuesTests::testTextValue,"Test parsing text");
	addTest((TestFunction)&SensorValuesTests::testDumpFunctions,"Test dump functions");
}

void SensorValuesTests::testSingleValue()
{
	SensorValueF32 valNT(singleNT.type);
	VERIFY(valNT.parseMsgArgs(singleData1MsgArgs))
	COMPARE(valNT.getSample(0),singleData1)

	SensorValueF32 valLT(singleLT.type);
	VERIFY(valLT.parseMsgArgs(singleData1MsgArgsWithTs))
	COMPARE(valLT.getSample(0),singleData1)
	COMPARE(valLT.time(),someTimestamp)

	SensorValueF32 valGT(singleGT.type);
	VERIFY(valGT.parseBinary(singleData1BinaryWithTs))
	COMPARE(valGT.getSample(0),singleData1)
	COMPARE(valGT.time(),someTimestamp)
}

void SensorValuesTests::testTextValue()
{
	SensorDef::Type type;
	type.dim=2;
	type.numType=SensorDef::TEXT;

	SensorValueText val(type);
	VERIFY(val.parseMsgArgs(textValueMsgArgs))
	COMPARE(val.getT(0),"text0")
	COMPARE(val.getT(1),"text1")
}

void SensorValuesTests::testPacketValue()
{
	SensorValueF32 valFloat(packetNT.type);
	VERIFY(valFloat.parseBinary(packetData1BinaryF))
	VERIFY(valFloat.packetsCount()==2)
	VERIFY(valFloat.getSample(0)==packetData1Sample0F)
	VERIFY(valFloat.getSample(1)==packetData1Sample1F)

	SensorDef::Type type=packetGT.type;
	type.numType=SensorDef::F64;
	SensorValueF64 valDouble(type);
	VERIFY(valDouble.parseMsgArgs(packetData1MsgArgsWithTs))
	VERIFY(valDouble.packetsCount()==2)
	VERIFY(valDouble.getSample(0)==packetData1Sample0D)
	VERIFY(valDouble.getSample(1)==packetData1Sample1D)
	VERIFY(valDouble.time()==someTimestamp)
}

void SensorValuesTests::testDumpFunctions()
{
	SensorValueF32 val1(singleNT.type),val2(singleNT.type);
	VERIFY(val1.parseMsgArgs(singleData1MsgArgs))
	VERIFY(val2.parseBinary(singleData1Binary))
	VERIFY(val1==val2)

	QByteArray data=val1.dumpToBinary();
	VERIFY(val2.parseBinary(data))
	VERIFY(val1==val2)

	SensorValueF32 val3(singleNT.type);
	QByteArrayList args=val1.dumpToMsgArgs();
	VERIFY(val3.parseMsgArgs(singleData1MsgArgs))
	VERIFY(val1==val3)
	VERIFY(val3.parseMsgArgs(args))
	VERIFY(val1==val3)
}
