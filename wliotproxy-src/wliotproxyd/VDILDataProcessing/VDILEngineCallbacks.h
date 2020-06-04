#ifndef VDILENGINECALLBACKS_H
#define VDILENGINECALLBACKS_H

#include "VDIL/core/IEngineCallbacks.h"

class VDILEngineCallbacks
	:public WLIOTVDIL::IEngineCallbacks
{
public:
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual void debugCallback(const QString &msg)override;
};

#endif // VDILENGINECALLBACKS_H
