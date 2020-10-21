#include "TestVDILPlugin.h"

TestVDILPlugin::TestVDILPlugin()
{
}

QUuid TestVDILPlugin::id()
{
	return "{7ea59747-e2a1-41a2-8934-6c0c479a8b53}";
}

QString TestVDILPlugin::name()
{
	return "test VDIL plugin";
}

QString TestVDILPlugin::developer()
{
	return "WL";
}

QString TestVDILPlugin::version()
{
	return "1.0.0";
}

QString TestVDILPlugin::description()
{
	return "does nothing";
}

void TestVDILPlugin::setup(WLIOTVDIL::Engine *e)
{
	Q_UNUSED(e)
}
