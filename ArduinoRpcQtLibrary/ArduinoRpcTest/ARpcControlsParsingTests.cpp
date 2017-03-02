#include "ARpcControlsParsingTests.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcUi/ARpcControlUi.h"
#include "FakeDevice.h"
#include <QDialog>
#include <QLayout>

static const QString jsonDescr=QString::fromUtf8(
	"{\"controls\":{\"element_type\":\"group\",\"title\":\"Test controls set\",\"elements\":"
	"[{\"element_type\":\"group\",\"title\":\"group1\",\"layout\":\"h\",\"elements\":"
	"[{\"element_type\":\"control\",\"title\":\"Light\",\"command\":\"light\",\"sync\":false,\"params\":"
	"[{\"title\":\"Light switch\",\"type\":\"checkbox\",\"constraints\":{\"onValue\":\"1\",\"offValue\":\"0\"}}]},"
	"{\"element_type\":\"control\",\"title\":\"Heater\",\"command\":\"heater\",\"sync\":true,\"params\":"
	"[{\"title\":\"Heater control\",\"type\":\"slider\",\"constraints\":{\"min\":\"0\",\"max\":\"100\"}}]}]},"
	"{\"element_type\":\"control\",\"title\":\"Reset\",\"command\":\"reset\",\"sync\":false}]}}"
);

static const QString xmlDescr=QString::fromUtf8(
	"<controls><group title=\"Test controls set\"><group title=\"group1\" layout=\"h\">"
	"<control title=\"Light\" command=\"light\" sync=\"0\">"
	"<param title=\"Light switch\" type=\"checkbox\"><constraints onValue=\"1\" offValue=\"0\"/></param></control>"
	"<control title=\"Heater\" command=\"heater\" sync=\"1\">"
	"<param title=\"Heater control\" type=\"slider\"><constraints min=\"0\" max=\"100\"/></param></control>"
	"</group><control title=\"Reset\" command=\"reset\" sync=\"0\"/></group></controls>"
);

static const QString xmlDescr2=QString::fromUtf8(
"<controls>"
"	<group title=\"Test controls set\">"
"		<group title=\"group1\" layout=\"h\">"
"			<control title=\"Light\" command=\"light\" sync=\"0\">"
"				<param title=\"Light switch\" type=\"checkbox\">"
"					<constraints onValue=\"1\" offValue=\"0\"/>"
"				</param>"
"			</control>"
"			<control title=\"Heater\" command=\"heater\" layout=\"h\" sync=\"1\">"
"				<param title=\"Heater power\" type=\"slider\">"
"					<constraints min=\"0\" max=\"100\"/>"
"				</param>"
"				<param title=\"Fan speed\" type=\"dial\">"
"					<constraints min=\"0\" max=\"100\"/>"
"				</param>"
"			</control>"
"		</group>"
"		<control title=\"Work mode\" command=\"set_mode\" sync=\"0\">"
"			<param title=\"Modes\" type=\"select\">"
"				<constraints values=\"m1;m2;m3;m4\"/>"
"			</param>"
"		</control>"
"		<control title=\"Reset\" command=\"reset\" sync=\"0\"/>"
"	</group>"
"</controls>"
);

//TODO check parsing control's layout

ARpcControlsParsingTests::ARpcControlsParsingTests(QObject *parent)
	:QtUnitTestSet(parent)
{
	addTest((TestFunction)&ARpcControlsParsingTests::testParseJson,"Test json parsing");
	addTest((TestFunction)&ARpcControlsParsingTests::testParseXml,"Test xml parsing");
}

void ARpcControlsParsingTests::testParseJson()
{
	ARpcControlsGroup controls;
	VERIFY(ARpcControlsGroup::parseJsonDescription(jsonDescr,controls))
	COMPARE(controls.title,"Test controls set")
	COMPARE(controls.layout,Qt::Vertical)
	VERIFY(controls.elements.count()==2)
	VERIFY(controls.elements[0].isGroup())
	VERIFY(controls.elements[1].isControl())
	VERIFY(!controls.elements[0].isControl())
	VERIFY(!controls.elements[1].isGroup())
	ARpcControlsGroup *g1=controls.elements[0].group();//group1
	VERIFY(g1!=0)
	COMPARE(g1->title,"group1")
	COMPARE(g1->layout,Qt::Horizontal)
	VERIFY(g1->elements.count()==2)
	VERIFY(g1->elements[0].isControl())
	VERIFY(g1->elements[1].isControl())
	ARpcCommandControl *c1=g1->elements[0].control();//light
	ARpcCommandControl *c2=g1->elements[1].control();//heater
	VERIFY(c1!=0&&c2!=0)
	COMPARE(c1->title,"Light")
	COMPARE(c1->command,"light")
	VERIFY(!c1->syncCall)
	VERIFY(c1->params.count()==1)
	COMPARE(c1->params[0].title,"Light switch")
	COMPARE(c1->params[0].type,ARpcControlParam::CHECKBOX)
	VERIFY(c1->params[0].constraints.count()==2)
	COMPARE(c1->params[0].constraints["onValue"],"1")
	COMPARE(c1->params[0].constraints["offValue"],"0")
	COMPARE(c2->title,"Heater")
	COMPARE(c2->command,"heater")
	VERIFY(c2->syncCall)
	VERIFY(c2->params.count()==1)
	COMPARE(c2->params[0].title,"Heater control")
	COMPARE(c2->params[0].type,ARpcControlParam::SLIDER)
	VERIFY(c2->params[0].constraints.count()==2)
	COMPARE(c2->params[0].constraints["min"],"0")
	COMPARE(c2->params[0].constraints["max"],"100")
	ARpcCommandControl *cc=controls.elements[1].control();//reset
	VERIFY(cc!=0)
	COMPARE(cc->title,"Reset")
	COMPARE(cc->command,"reset")
	VERIFY(!cc->syncCall)
	VERIFY(cc->params.count()==0)
}

void ARpcControlsParsingTests::testParseXml()
{
	ARpcControlsGroup controls;
	VERIFY(ARpcControlsGroup::parseXmlDescription(xmlDescr,controls))
	COMPARE(controls.title,"Test controls set")
	COMPARE(controls.layout,Qt::Vertical)
	VERIFY(controls.elements.count()==2)
	VERIFY(controls.elements[0].isGroup())
	VERIFY(controls.elements[1].isControl())
	VERIFY(!controls.elements[0].isControl())
	VERIFY(!controls.elements[1].isGroup())
	ARpcControlsGroup *g1=controls.elements[0].group();//group1
	VERIFY(g1!=0)
	COMPARE(g1->title,"group1")
	COMPARE(g1->layout,Qt::Horizontal)
	VERIFY(g1->elements.count()==2)
	VERIFY(g1->elements[0].isControl())
	VERIFY(g1->elements[1].isControl())
	ARpcCommandControl *c1=g1->elements[0].control();//light
	ARpcCommandControl *c2=g1->elements[1].control();//heater
	VERIFY(c1!=0&&c2!=0)
	COMPARE(c1->title,"Light")
	COMPARE(c1->command,"light")
	VERIFY(!c1->syncCall)
	VERIFY(c1->params.count()==1)
	COMPARE(c1->params[0].title,"Light switch")
	COMPARE(c1->params[0].type,ARpcControlParam::CHECKBOX)
	VERIFY(c1->params[0].constraints.count()==2)
	COMPARE(c1->params[0].constraints["onValue"],"1")
	COMPARE(c1->params[0].constraints["offValue"],"0")
	COMPARE(c2->title,"Heater")
	COMPARE(c2->command,"heater")
	VERIFY(c2->syncCall)
	VERIFY(c2->params.count()==1)
	COMPARE(c2->params[0].title,"Heater control")
	COMPARE(c2->params[0].type,ARpcControlParam::SLIDER)
	VERIFY(c2->params[0].constraints.count()==2)
	COMPARE(c2->params[0].constraints["min"],"0")
	COMPARE(c2->params[0].constraints["max"],"100")
	ARpcCommandControl *cc=controls.elements[1].control();//reset
	VERIFY(cc!=0)
	COMPARE(cc->title,"Reset")
	COMPARE(cc->command,"reset")
	VERIFY(!cc->syncCall)
	VERIFY(cc->params.count()==0)

	VERIFY(ARpcControlsGroup::parseXmlDescription(xmlDescr2,controls))
	FakeDevice dev;
	QDialog dlg;
	ARpcControlUi *ui=new ARpcControlUi(&dev,controls);
	QVBoxLayout *l=new QVBoxLayout(&dlg);
	l->addWidget(ui);
	dlg.exec();
}
