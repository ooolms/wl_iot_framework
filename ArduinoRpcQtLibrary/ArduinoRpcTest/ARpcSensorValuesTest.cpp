#include "ARpcSensorValuesTest.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"

static const ARpcMessage singleValue=ARpcMessage("meas",QStringList()<<"sensorName"<<"12.0"<<"54.3"<<"135.1");
static const ARpcMessage singleValueWithTime=
	ARpcMessage("meas",QStringList()<<"sensorName"<<"15391"<<"12.0"<<"54.3"<<"135.1");
static const float packetValue[]={12.0,54.3,135.1,20.5,56.1,63.6};
static const ARpcMessage textValue=ARpcMessage("meas",QStringList()<<"sensorName"<<"text1"<<"text2");
static const QVector<QVector<float>> packetParsed=QVector<QVector<float>>()<<
	(QVector<float>()<<12.0<<54.3<<135.1)<<
	(QVector<float>()<<20.5<<56.1<<63.6);

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
	COMPARE(valNoTime.values(),packetParsed)
	m.args.insert(1,"15391");
	ARpcPacketSensorValue valLocalTime(3,true);
	VERIFY(valLocalTime.type()==ARpcSensor::PACKET_LT)
	VERIFY(valLocalTime.parse(m))
	COMPARE(valLocalTime.values(),packetParsed)
	COMPARE(valLocalTime.time(),15391)
	ARpcPacketSensorValue valGlobalTime(3,false);
	VERIFY(valGlobalTime.type()==ARpcSensor::PACKET_GT)
	VERIFY(valGlobalTime.parse(m))
	COMPARE(valGlobalTime.values(),packetParsed)
	COMPARE(valGlobalTime.time(),15391)
}

