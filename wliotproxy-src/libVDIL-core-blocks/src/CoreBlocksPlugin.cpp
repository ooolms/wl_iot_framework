#include "VDIL/CoreBlocksPlugin.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/core/LogicalBlocksGroupFactory.h"
#include "VDIL/xml/CoreBlocksGroupXmlParserFactory.h"
#include "VDIL/xml/LogicalBlocksGroupXmlParserFactory.h"
#include "VDIL/core/ProgramStoragesRuntime.h"
#include "VDIL/core/ProgramDevicesRuntime.h"
#include "VDIL/core/Engine.h"

using namespace WLIOTVDIL;

template<class C>
void regGrpFact(Engine *e)
{
	C *f=new C;
	if(!e->bf.registerGroupFactory(f))
		delete f;
}

template<class C>
void regGrpXmlFact(Engine *e)
{
	C *f=new C;
	if(!e->bxpf.registerGroupFactory(f))
		delete f;
}

QUuid CoreBlocksPlugin::id()
{
	return QUuid("{4e237878-21d4-486c-b7c9-9d0ca1ce66c6}");
}

QString CoreBlocksPlugin::name()
{
	return "Core plugin";
}

QString CoreBlocksPlugin::developer()
{
	return "WL";
}

QString CoreBlocksPlugin::version()
{
	return "1.0.0";
}

QString CoreBlocksPlugin::description()
{
	return "Core blocks set for VDIL language";
}

void CoreBlocksPlugin::setup(Engine *e)
{
	regGrpFact<CoreBlocksGroupFactory>(e);
	regGrpFact<LogicalBlocksGroupFactory>(e);
	regGrpXmlFact<CoreBlocksGroupXmlParserFactory>(e);
	regGrpXmlFact<LogicalBlocksGroupXmlParserFactory>(e);
	e->prf.registerRuntime(new ProgramDevicesRuntime);
	e->prf.registerRuntime(new ProgramStoragesRuntime);
}
