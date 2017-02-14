#include "ARpcSensorsParsingTests.h"
#include "ARpcBase/ARpcSensor.h"

static const QString jsonDescr=QString::fromUtf8("{\"sensors\":[{\"name\":\"humidity\",\"type\":\"float\","
	"\"constraints\":{\"min\":\"0\",\"max\":\"100\",\"resolution\":\"1\"}},"
	"{\"name\":\"temperature\",\"type\":\"integer\"}]}");
static const QString xmlDescr=QString::fromUtf8(
	"<sensors><sensor name=\"humidity\" type=\"float\">"
	"<constraints min=\"0\" max=\"100\" resolution=\"1\"/>"
	"</sensor><sensor name=\"temperature\" type=\"integer\"/></sensors>");

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
	COMPARE(sensors[0].name,"humidity")
	COMPARE(sensors[0].type,ARpcSensor::FLOAT)
	COMPARE(sensors[0].constraints.count(),3)
	COMPARE(sensors[0].constraints["min"],"0")
	COMPARE(sensors[0].constraints["max"],"100")
	COMPARE(sensors[0].constraints["resolution"],"1")
	COMPARE(sensors[1].name,"temperature")
	COMPARE(sensors[1].type,ARpcSensor::INTEGER)
}

void ARpcSensorsParsingTests::testParseXml()
{
	QList<ARpcSensor> sensors;
	VERIFY(ARpcSensor::parseXmlDescription(xmlDescr,sensors));
	VERIFY(sensors.count()==2)
	COMPARE(sensors[0].name,"humidity")
	COMPARE(sensors[0].type,ARpcSensor::FLOAT)
	COMPARE(sensors[0].constraints.count(),3)
	COMPARE(sensors[0].constraints["min"],"0")
	COMPARE(sensors[0].constraints["max"],"100")
	COMPARE(sensors[0].constraints["resolution"],"1")
	COMPARE(sensors[1].name,"temperature")
	COMPARE(sensors[1].type,ARpcSensor::INTEGER)
}
