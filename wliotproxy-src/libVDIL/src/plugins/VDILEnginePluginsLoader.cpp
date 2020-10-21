#include "VDIL/plugins/VDILEnginePluginsLoader.h"
#include "VDIL/core/Engine.h"
#include <QDir>
#include <QLibrary>

#ifdef Q_OS_WIN
static QString fileFilter="*.dll";
#else
static QString fileFilter="*.so";
#endif

WLIOTVDIL::VDILEnginePluginsLoader::VDILEnginePluginsLoader(WLIOTVDIL::Engine *e)
{
	engine=e;
}

WLIOTVDIL::VDILEnginePluginsLoader::~VDILEnginePluginsLoader()
{
	for(auto p:mPlugins)
		delete p;
}

bool WLIOTVDIL::VDILEnginePluginsLoader::registerPlugin(WLIOTVDIL::VDILEnginePlugin *p)
{
	if(p->id().isNull())return false;
	if(mPlugins.contains(p->id()))return false;
	mPlugins[p->id()]=p;
	p->setup(engine);
	return true;
}

void WLIOTVDIL::VDILEnginePluginsLoader::loadPlugins(const QString &path)
{
	QDir dir(path);
	QStringList libs=dir.entryList(QStringList()<<fileFilter,QDir::Files);
	for(auto &s:libs)
	{
		QLibrary lib(dir.absoluteFilePath(s));
		VDILPluginLoadFunction f=(VDILPluginLoadFunction)lib.resolve(VDILPluginLoadFunctionName);
		if(!f)continue;
		VDILEnginePlugin *p=f();
		if(!p)continue;
		if(!registerPlugin(p))
		{
			delete p;
			lib.unload();
		}
	}
}
