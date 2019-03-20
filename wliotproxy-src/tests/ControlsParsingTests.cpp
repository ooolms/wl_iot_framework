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

#include "ControlsParsingTests.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/ControlUi.h"
#include "FakeDevice.h"
#include <QDialog>
#include <QLayout>

static const QByteArray jsonDescr=
	"{\"controls\":{\"element_type\":\"group\",\"title\":\"Test controls set\",\"elements\":"
	"[{\"element_type\":\"group\",\"title\":\"group1\",\"layout\":\"h\",\"elements\":"
	"[{\"element_type\":\"control\",\"title\":\"Light\",\"command\":\"light\",\"params\":"
	"[{\"title\":\"Light switch\",\"type\":\"checkbox\",\"attributes\":{\"onValue\":\"1\",\"offValue\":\"0\"}}]},"
	"{\"element_type\":\"control\",\"title\":\"Heater\",\"command\":\"heater\",\"params\":"
	"[{\"title\":\"Heater control\",\"type\":\"slider\",\"attributes\":{\"min\":\"0\",\"max\":\"100\"}}]}]},"
	"{\"element_type\":\"control\",\"title\":\"Reset\",\"command\":\"reset\"}]}}";

static const QByteArray xmlDescr=
	"<controls><group title=\"Test controls set\"><group title=\"group1\" layout=\"h\">"
	"<control title=\"Light\" command=\"light\">"
	"<param title=\"Light switch\" type=\"checkbox\"><attributes onValue=\"1\" offValue=\"0\"/></param></control>"
	"<control title=\"Heater\" command=\"heater\">"
	"<param title=\"Heater control\" type=\"slider\"><attributes min=\"0\" max=\"100\"/></param></control>"
	"</group><control title=\"Reset\" command=\"reset\"/></group></controls>";

static const QByteArray xmlDescr2=
"<controls>"
"	<group title=\"Test controls set\">"
"		<group title=\"group1\" layout=\"h\">"
"			<control title=\"Light\" command=\"light\">"
"				<param title=\"Light switch\" type=\"checkbox\">"
"					<attributes onValue=\"1\" offValue=\"0\"/>"
"				</param>"
"			</control>"
"			<control title=\"Heater\" command=\"heater\" layout=\"h\">"
"				<param title=\"Heater power\" type=\"slider\">"
"					<attributes min=\"0\" max=\"100\"/>"
"				</param>"
"				<param title=\"Fan speed\" type=\"dial\">"
"					<attributes min=\"0\" max=\"100\"/>"
"				</param>"
"			</control>"
"		</group>"
"		<control title=\"Work mode\" command=\"set_mode\">"
"			<param title=\"Modes\" type=\"select\">"
"				<attributes values=\"m1;m2;m3;m4\"/>"
"			</param>"
"		</control>"
"		<control title=\"Reset\" command=\"reset\"/>"
"	</group>"
"</controls>";

//TODO check parsing control's layout

ControlsParsingTests::ControlsParsingTests(QObject *parent)
	:QtUnitTestSet("ControlsParsingTests",parent)
{
	addTest((TestFunction)&ControlsParsingTests::testParseJson,"Test json parsing");
	addTest((TestFunction)&ControlsParsingTests::testParseXml,"Test xml parsing");
}

void ControlsParsingTests::testParseJson()
{
	ControlsGroup controls;
	VERIFY(ControlsGroup::parseJsonDescription(jsonDescr,controls))
	COMPARE(controls.title,"Test controls set");
	COMPARE(controls.layout,Qt::Vertical);
	VERIFY(controls.elements.count()==2);
	VERIFY(controls.elements[0].isGroup());
	VERIFY(controls.elements[1].isControl());
	VERIFY(!controls.elements[0].isControl());
	VERIFY(!controls.elements[1].isGroup());
	ControlsGroup *g1=controls.elements[0].group();//group1
	VERIFY(g1!=0);
	COMPARE(g1->title,"group1");
	COMPARE(g1->layout,Qt::Horizontal);
	VERIFY(g1->elements.count()==2);
	VERIFY(g1->elements[0].isControl());
	VERIFY(g1->elements[1].isControl());
	CommandControl *c1=g1->elements[0].control();//light
	CommandControl *c2=g1->elements[1].control();//heater
	VERIFY(c1!=0&&c2!=0);
	COMPARE(c1->title,"Light");
	COMPARE(c1->command,"light");
	VERIFY(c1->params.count()==1);
	COMPARE(c1->params[0].title,"Light switch");
	COMPARE(c1->params[0].type,ControlParam::CHECKBOX);
	VERIFY(c1->params[0].attributes.count()==2);
	COMPARE(c1->params[0].attributes["onValue"],"1");
	COMPARE(c1->params[0].attributes["offValue"],"0");
	COMPARE(c2->title,"Heater");
	COMPARE(c2->command,"heater");
	VERIFY(c2->params.count()==1);
	COMPARE(c2->params[0].title,"Heater control");
	COMPARE(c2->params[0].type,ControlParam::SLIDER);
	VERIFY(c2->params[0].attributes.count()==2);
	COMPARE(c2->params[0].attributes["min"],"0");
	COMPARE(c2->params[0].attributes["max"],"100");
	CommandControl *cc=controls.elements[1].control();//reset
	VERIFY(cc!=0);
	COMPARE(cc->title,"Reset");
	COMPARE(cc->command,"reset");
	VERIFY(cc->params.count()==0);
}

void ControlsParsingTests::testParseXml()
{
	ControlsGroup controls;
	VERIFY(ControlsGroup::parseXmlDescription(xmlDescr,controls));
	COMPARE(controls.title,"Test controls set");
	COMPARE(controls.layout,Qt::Vertical);
	VERIFY(controls.elements.count()==2);
	VERIFY(controls.elements[0].isGroup());
	VERIFY(controls.elements[1].isControl());
	VERIFY(!controls.elements[0].isControl());
	VERIFY(!controls.elements[1].isGroup());
	ControlsGroup *g1=controls.elements[0].group();//group1
	VERIFY(g1!=0);
	COMPARE(g1->title,"group1");
	COMPARE(g1->layout,Qt::Horizontal);
	VERIFY(g1->elements.count()==2);
	VERIFY(g1->elements[0].isControl());
	VERIFY(g1->elements[1].isControl());
	CommandControl *c1=g1->elements[0].control();//light
	CommandControl *c2=g1->elements[1].control();//heater
	VERIFY(c1!=0&&c2!=0);
	COMPARE(c1->title,"Light");
	COMPARE(c1->command,"light");
	VERIFY(c1->params.count()==1);
	COMPARE(c1->params[0].title,"Light switch");
	COMPARE(c1->params[0].type,ControlParam::CHECKBOX);
	VERIFY(c1->params[0].attributes.count()==2);
	COMPARE(c1->params[0].attributes["onValue"],"1");
	COMPARE(c1->params[0].attributes["offValue"],"0");
	COMPARE(c2->title,"Heater");
	COMPARE(c2->command,"heater");
	VERIFY(c2->params.count()==1);
	COMPARE(c2->params[0].title,"Heater control");
	COMPARE(c2->params[0].type,ControlParam::SLIDER);
	VERIFY(c2->params[0].attributes.count()==2);
	COMPARE(c2->params[0].attributes["min"],"0");
	COMPARE(c2->params[0].attributes["max"],"100");
	CommandControl *cc=controls.elements[1].control();//reset
	VERIFY(cc!=0);
	COMPARE(cc->title,"Reset");
	COMPARE(cc->command,"reset");
	VERIFY(cc->params.count()==0);

//	VERIFY(ControlsGroup::parseXmlDescription(xmlDescr2,controls));
//	FakeDevice dev;
//	QDialog dlg;
//	ControlUi *ui=new ControlUi(&dev,controls);
//	QVBoxLayout *l=new QVBoxLayout(&dlg);
//	l->addWidget(ui);
//	dev.msgFromDevice(Message(Config::stateChangedMsg,QStringList()<<"heater"<<"1"<<"50"));
//	dlg.exec();
}
