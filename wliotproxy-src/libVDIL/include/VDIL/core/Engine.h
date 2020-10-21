#ifndef ENGINE_H
#define ENGINE_H

#include "VDIL/core/BlocksFactory.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/core/ProgramRuntimesFactory.h"
#include "VDIL/plugins/VDILEnginePluginsLoader.h"

namespace WLIOTVDIL
{
	class Engine
	{
	public:
		Engine();

	public:
		BlocksFactory bf;
		BlocksXmlParserFactory bxpf;
		ProgramRuntimesFactory prf;
		VDILEnginePluginsLoader plg;
	};
}

#endif // ENGINE_H
