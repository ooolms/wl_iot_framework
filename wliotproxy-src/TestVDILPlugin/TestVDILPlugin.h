#ifndef TESTVDILPLUGIN_H
#define TESTVDILPLUGIN_H

#include <VDIL/plugins/VDILEnginePlugin.h>

class TestVDILPlugin
	:public WLIOTVDIL::VDILEnginePlugin
{
public:
	TestVDILPlugin();
	virtual QUuid id()override;
	virtual QString name()override;
	virtual QString developer()override;
	virtual QString version()override;
	virtual QString description()override;
	virtual void setup(WLIOTVDIL::Engine *e)override;
};

#endif // TESTVDILPLUGIN_H
