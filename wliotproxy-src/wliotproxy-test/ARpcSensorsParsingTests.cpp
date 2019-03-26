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

#include "ARpcSensorsParsingTests.h"
#include "ARpcBase/ARpcSensorDef.h"

static const QByteArray jsonDescr="{\"sensors\":[{\"name\":\"humidity\",\"type\":\"f32_sv_d2\"},"
	"{\"name\":\"temperature\",\"type\":\"u64_pv_lt\"}]}";
static const QByteArray xmlDescr=
	"<sensors>"
	"<sensor name=\"humidity\" type=\"f32_sv_d2\"/>"
	"<sensor name=\"temperature\" type=\"u64_pv_lt\"/>"
	"</sensors>";

ARpcSensorsParsingTests::ARpcSensorsParsingTests(QObject *parent)
	:QtUnitTestSet("ARpcSensorsParsingTests",parent)
{
	addTest((TestFunction)&ARpcSensorsParsingTests::testParseJson,"Test json parsing");
	addTest((TestFunction)&ARpcSensorsParsingTests::testParseXml,"Test xml parsing");
}

void ARpcSensorsParsingTests::testParseJson()
{
	QList<ARpcSensorDef> sensors;
	VERIFY(ARpcSensorDef::parseJsonDescription(jsonDescr,sensors));
	VERIFY(sensors.count()==2)
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type.packType,ARpcSensorDef::SINGLE);
	COMPARE(sensors[0].type.dim,2);
	COMPARE(sensors[0].type.numType,ARpcSensorDef::F32);
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type.packType,ARpcSensorDef::PACKET);
	COMPARE(sensors[1].type.dim,1);
	COMPARE(sensors[1].type.numType,ARpcSensorDef::U64);
}

void ARpcSensorsParsingTests::testParseXml()
{
	QList<ARpcSensorDef> sensors;
	VERIFY(ARpcSensorDef::parseXmlDescription(xmlDescr,sensors));
	VERIFY(sensors.count()==2);
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type.packType,ARpcSensorDef::SINGLE);
	COMPARE(sensors[0].type.dim,2);
	COMPARE(sensors[0].type.numType,ARpcSensorDef::F32);
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type.packType,ARpcSensorDef::PACKET);
	COMPARE(sensors[1].type.dim,1);
	COMPARE(sensors[1].type.numType,ARpcSensorDef::U64);
}