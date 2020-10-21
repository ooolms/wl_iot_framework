#include "VDIL/core/ProgramRuntimesFactory.h"
#include "VDIL/core/Program.h"

void WLIOTVDIL::ProgramRuntimesFactory::registerRuntime(WLIOTVDIL::IProgramRuntime *r)
{
	mRuntimes.insert(r);
}

void WLIOTVDIL::ProgramRuntimesFactory::setup(WLIOTVDIL::Program *p)
{
	for(auto r:mRuntimes)
		p->addRuntime(r->mkRuntime(p));
}

const QSet<WLIOTVDIL::IProgramRuntime*> WLIOTVDIL::ProgramRuntimesFactory::runtimes()const
{
	return mRuntimes;
}
