#include "ARpcSensorsParsingTests.h"
#include "ARpcBase/ARpcSensor.h"

static const QString jsonDescr=QString::fromUtf8("{\"sensors\":[{\"name\":\"humidity\",\"type\":\"single\","
	"\"constraints\":{\"dims\":\"2\"}},""{\"name\":\"temperature\",\"type\":\"single_lt\"}]}");
static const QString xmlDescr=QString::fromUtf8(
	"<sensors><sensor name=\"humidity\" type=\"single\">"
	"<constraints dims=\"2\"/>"
	"</sensor><sensor name=\"temperature\" type=\"single_lt\"/></sensors>");

ARpcSensorsParsingTests::ARpcSensorsParsingTests(QObject *parent)
	:QtUnitTestSet("ARpcSensorsParsingTests",parent)
{
	addTest((TestFunction)&ARpcSensorsParsingTests::testParseJson,"Test json parsing");
	addTest((TestFunction)&ARpcSensorsParsingTests::testParseXml,"Test xml parsing");
}

void ARpcSensorsParsingTests::testParseJson()
{
	QList<ARpcSensor> sensors;
	VERIFY(ARpcSensor::parseJsonDescription(jsonDescr,sensors));
	VERIFY(sensors.count()==2)
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type,ARpcSensor::SINGLE);
	COMPARE(sensors[0].constraints.count(),1);
	COMPARE(sensors[0].constraints["dims"],"2");
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type,ARpcSensor::SINGLE_LT);
}

void ARpcSensorsParsingTests::testParseXml()
{
	QList<ARpcSensor> sensors;
	VERIFY(ARpcSensor::parseXmlDescription(xmlDescr,sensors));
	VERIFY(sensors.count()==2);
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type,ARpcSensor::SINGLE);
	COMPARE(sensors[0].constraints.count(),1);
	COMPARE(sensors[0].constraints["dims"],"2");
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type,ARpcSensor::SINGLE_LT);
}
