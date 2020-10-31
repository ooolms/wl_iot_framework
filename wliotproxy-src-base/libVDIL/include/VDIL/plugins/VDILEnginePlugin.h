#ifndef VDILENGINEPLUGIN_H
#define VDILENGINEPLUGIN_H

#include <QUuid>

namespace WLIOTVDIL
{
	class Engine;

	class VDILEnginePlugin
	{
	public:
		virtual ~VDILEnginePlugin(){}
		virtual QUuid id()=0;
		virtual QString name()=0;
		virtual QString developer()=0;
		virtual QString version()=0;
		virtual QString description()=0;
		virtual void setup(Engine *e)=0;
	};

}

typedef WLIOTVDIL::VDILEnginePlugin* (*VDILPluginLoadFunction)();

#define VDILPluginLoadFunctionName "vdilMakePlugin"
#define VDIL_MAKE_PLUGIN(ClassName)\
extern "C" WLIOTVDIL::VDILEnginePlugin* vdilMakePlugin(){return new ClassName;}

#endif // VDILENGINEPLUGIN_H
