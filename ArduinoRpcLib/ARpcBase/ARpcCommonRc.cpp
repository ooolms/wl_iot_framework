#include "ARpcCommonRc.h"
#include <QtGlobal>

bool ARpcCommonRc::done=false;

void ARpcCommonRc::initRc()
{
	if(!done)
	{
		Q_INIT_RESOURCE(ARpcCommon);
		done=true;
	}
}
