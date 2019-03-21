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

#include "SensorsParsingTests.h"
#include "wliot/devices/SensorDef.h"

static const QByteArray jsonDescr="{\"sensors\":[{\"name\":\"humidity\",\"type\":\"f32_sv_d2\"},"
	"{\"name\":\"temperature\",\"type\":\"u64_pv_lt\"}]}";
static const QByteArray xmlDescr=
	"<sensors>"
	"<sensor name=\"humidity\" type=\"f32_sv_d2\"/>"
	"<sensor name=\"temperature\" type=\"u64_pv_lt\"/>"
	"</sensors>";

SensorsParsingTests::SensorsParsingTests(QObject *parent)
	:QtUnitTestSet("SensorsParsingTests",parent)
{
	addTest((TestFunction)&SensorsParsingTests::testParseJson,"Test json parsing");
	addTest((TestFunction)&SensorsParsingTests::testParseXml,"Test xml parsing");
}

void SensorsParsingTests::testParseJson()
{
	QList<SensorDef> sensors;
	VERIFY(SensorDef::parseJsonDescription(jsonDescr,sensors));
	VERIFY(sensors.count()==2)
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type.packType,SensorDef::SINGLE);
	COMPARE(sensors[0].type.dim,2);
	COMPARE(sensors[0].type.numType,SensorDef::F32);
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type.packType,SensorDef::PACKET);
	COMPARE(sensors[1].type.dim,1);
	COMPARE(sensors[1].type.numType,SensorDef::U64);
}

void SensorsParsingTests::testParseXml()
{
	QList<SensorDef> sensors;
	VERIFY(SensorDef::parseXmlDescription(xmlDescr,sensors));
	VERIFY(sensors.count()==2);
	COMPARE(sensors[0].name,"humidity");
	COMPARE(sensors[0].type.packType,SensorDef::SINGLE);
	COMPARE(sensors[0].type.dim,2);
	COMPARE(sensors[0].type.numType,SensorDef::F32);
	COMPARE(sensors[1].name,"temperature");
	COMPARE(sensors[1].type.packType,SensorDef::PACKET);
	COMPARE(sensors[1].type.dim,1);
	COMPARE(sensors[1].type.numType,SensorDef::U64);
}
