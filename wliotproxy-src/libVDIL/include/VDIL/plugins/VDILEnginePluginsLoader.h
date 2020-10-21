#ifndef VDILENGINEPLUGINSLOADER_H
#define VDILENGINEPLUGINSLOADER_H

#include "VDIL/plugins/VDILEnginePlugin.h"
#include <QMap>

namespace WLIOTVDIL
{
	class VDILEnginePluginsLoader
	{
	public:
		explicit VDILEnginePluginsLoader(Engine *e);
		virtual ~VDILEnginePluginsLoader();
		bool registerPlugin(VDILEnginePlugin *p);
		void loadPlugins(const QString &path);

	private:
		Engine *engine;
		QMap<QUuid,VDILEnginePlugin*> mPlugins;
	};
}

#endif // VDILENGINEPLUGINSLOADER_H
