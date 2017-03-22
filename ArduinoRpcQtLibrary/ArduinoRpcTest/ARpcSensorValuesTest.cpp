#include "ARpcSensorValuesTest.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"

static const ARpcMessage singleValue=ARpcMessage("meas",QStringList()<<"sensorName"<<"12.0"<<"54.3"<<"135.1");
static const ARpcMessage singleValueWithTime=
	ARpcMessage("meas",QStringList()<<"sensorName"<<"15391"<<"12.0"<<"54.3"<<"135.1");
static const float packetValue[]={12.0,54.3,135.1,20.5,56.1,63.6};
static const ARpcMessage textValue=ARpcMessage("meas",QStringList()<<"sensorName"<<"text1"<<"text2");

ARpcSensorValuesTest::ARpcSensorValuesTest(QObject *parent)
	:QtUnitTestSet("ARpcSensorValuesTest",parent)
{
	addTest((TestFunction)&ARpcSensorValuesTest::testSingleValue,"Test parsing single measurement");
	addTest((TestFunction)&ARpcSensorValuesTest::testPacketValue,"Test parsing packet of measurements");
	addTest((TestFunction)&ARpcSensorValuesTest::testTextValue,"Test parsing text");
}

void ARpcSensorValuesTest::testSingleValue()
{
	ARpcSingleSensorValue valNoTime(3);
	VERIFY(valNoTime.type()==ARpcSensor::SINGLE)
	VERIFY(valNoTime.parse(singleValue))
	COMPARE(valNoTime.values(),QVector<double>()<<12.0<<54.3<<135.1)
	ARpcSingleSensorValue valLocalTime(3,true);
	VERIFY(valLocalTime.type()==ARpcSensor::SINGLE_LT)
	VERIFY(valLocalTime.parse(singleValueWithTime))
	COMPARE(valLocalTime.values(),QVector<double>()<<12.0<<54.3<<135.1)
	COMPARE(valLocalTime.time(),15391)
	ARpcSingleSensorValue valGlobalTime(3,false);
	VERIFY(valGlobalTime.type()==ARpcSensor::SINGLE_GT)
	VERIFY(valGlobalTime.parse(singleValueWithTime))
	COMPARE(valGlobalTime.values(),QVector<double>()<<12.0<<54.3<<135.1)
	COMPARE(valGlobalTime.time(),15391)
}

void ARpcSensorValuesTest::testTextValue()
{
	ARpcTextSensorValue val;
	VERIFY(val.parse(textValue))
	COMPARE(val.value(),"text1 text2")
}

void ARpcSensorValuesTest::testPacketValue()
{
	ARpcMessage m;
	m.title="meas";
	m.args.append("sensorName");
	m.args.append(QString::fromUtf8(QByteArray((const char*)packetValue,sizeof(packetValue)).toBase64()));
	ARpcPacketSensorValue valNoTime(3);
	VERIFY(valNoTime.type()==ARpcSensor::PACKET)
	VERIFY(valNoTime.parse(m))
	VERIFY(valNoTime.valuesCount()==2)
	VERIFY(memcmp(valNoTime.values(),packetValue,valNoTime.dims()*valNoTime.valuesCount()*sizeof(float))==0)
	m.args.insert(1,"15391");
	ARpcPacketSensorValue valLocalTime(3,true);
	VERIFY(valLocalTime.type()==ARpcSensor::PACKET_LT)
	VERIFY(valLocalTime.parse(m))
	VERIFY(valLocalTime.valuesCount()==2)
	VERIFY(memcmp(valLocalTime.values(),packetValue,valLocalTime.dims()*valLocalTime.valuesCount()*sizeof(float))==0)
	COMPARE(valLocalTime.time(),15391)
	ARpcPacketSensorValue valGlobalTime(3,false);
	VERIFY(valGlobalTime.type()==ARpcSensor::PACKET_GT)
	VERIFY(valGlobalTime.parse(m))
	VERIFY(valGlobalTime.valuesCount()==2)
	VERIFY(memcmp(valGlobalTime.values(),packetValue,valGlobalTime.dims()*valGlobalTime.valuesCount()*sizeof(float))==0)
	COMPARE(valGlobalTime.time(),15391)
}

