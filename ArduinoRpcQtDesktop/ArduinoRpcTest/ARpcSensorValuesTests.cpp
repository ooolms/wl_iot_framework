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
#include "ARpcBase/ARpcSensorValue.h"
#include "TestData.h"
#include <QDateTime>
#include <QVector>

ARpcSensorValuesTests::ARpcSensorValuesTests(QObject *parent)
	:QtUnitTestSet("ARpcSensorValuesTest",parent)
{
	addTest((TestFunction)&ARpcSensorValuesTests::testSingleValue,"Test parsing single measurement");
	addTest((TestFunction)&ARpcSensorValuesTests::testPacketValue,"Test parsing packet of measurements");
	addTest((TestFunction)&ARpcSensorValuesTests::testTextValue,"Test parsing text");
	addTest((TestFunction)&ARpcSensorValuesTests::testDumpFunctions,"Test dump functions");
}

void ARpcSensorValuesTests::testSingleValue()
{
	ARpcSensorValueF32 valNT(singleNT.type);
	VERIFY(valNT.parseMsgArgs(singleData1MsgArgs));
	COMPARE(valNT.getSample(),singleData1);

	ARpcSensorValueF32 valLT(singleLT.type);
	VERIFY(valLT.parseMsgArgs(singleData1MsgArgsWithTs));
	COMPARE(valLT.getSample(),singleData1);
	COMPARE(valLT.time(),someTimestamp);

	ARpcSensorValueF32 valGT(singleGT.type);
	VERIFY(valGT.parseBinary(singleData1BinaryWithTs));
	COMPARE(valGT.getSample(),singleData1);
	COMPARE(valGT.time(),someTimestamp);
}

void ARpcSensorValuesTests::testTextValue()
{
	ARpcSensorDef::Type type;
	type.dim=2;
	type.numType=ARpcSensorDef::TEXT;

	ARpcSensorValueText val(type);
	VERIFY(val.parseMsgArgs(textValueMsgArgs));
	COMPARE(val.get(0),"text0");
	COMPARE(val.get(1),"text1");
}

void ARpcSensorValuesTests::testPacketValue()
{
	ARpcSensorValueF32 valFloat(packetNT.type);
	VERIFY(valFloat.parseBinary(packetData1BinaryF));
	VERIFY(valFloat.packetsCount()==2);
	VERIFY(valFloat.getSample(0)==packetData1Sample0F);
	VERIFY(valFloat.getSample(1)==packetData1Sample1F);

	ARpcSensorDef::Type type=packetGT.type;
	type.numType=ARpcSensorDef::F64;
	ARpcSensorValueF64 valDouble(type);
	VERIFY(valDouble.parseMsgArgs(packetData1MsgArgsWithTs));
	VERIFY(valDouble.packetsCount()==2);
	VERIFY(valDouble.getSample(0)==packetData1Sample0D);
	VERIFY(valDouble.getSample(1)==packetData1Sample1D);
	VERIFY(valDouble.time()==someTimestamp);
}

void ARpcSensorValuesTests::testDumpFunctions()
{
	ARpcSensorValueF32 val1(singleNT.type),val2(singleNT.type);
	VERIFY(val1.parseMsgArgs(singleData1MsgArgs))
	VERIFY(val2.parseBinary(singleData1Binary))
	VERIFY(val1==val2)

	QByteArray data=val1.dumpToBinary();
	VERIFY(val2.parseBinary(data))
	VERIFY(val1==val2)

	ARpcSensorValueF32 val3(singleNT.type);
	QByteArrayList args=val1.dumpToMsgArgs();
	VERIFY(val3.parseMsgArgs(singleData1MsgArgs))
	VERIFY(val1==val3)
	VERIFY(val3.parseMsgArgs(args))
	VERIFY(val1==val3)
}
