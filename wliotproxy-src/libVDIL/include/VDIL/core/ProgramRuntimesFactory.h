#ifndef PROGRAMRUNTIMESFACTORY_H
#define PROGRAMRUNTIMESFACTORY_H

#include "VDIL/core/IProgramRuntime.h"
#include <QSet>

namespace WLIOTVDIL
{
	class ProgramRuntimesFactory
	{
	public:
		void registerRuntime(IProgramRuntime *r);
		void setup(Program *p);
		const QSet<IProgramRuntime*> runtimes()const;

	private:
		QSet<IProgramRuntime*> mRuntimes;
	};
}

#endif // PROGRAMRUNTIMESFACTORY_H
