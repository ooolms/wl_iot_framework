#ifndef COREBLOCKSPLUGIN_H
#define COREBLOCKSPLUGIN_H

#include "VDIL/plugins/VDILEnginePlugin.h"

namespace WLIOTVDIL
{
	class CoreBlocksPlugin
		:public VDILEnginePlugin
	{
	public:
		virtual QUuid id()override;
		virtual QString name()override;
		virtual QString developer()override;
		virtual QString version()override;
		virtual QString description()override;
		virtual void setup(Engine *e)override;
	};
}

#endif // COREBLOCKSPLUGIN_H
