#ifndef GDILENGINECALLBACKS_H
#define GDILENGINECALLBACKS_H

#include "GDIL/core/IEngineCallbacks.h"

class GDILEngineCallbacks
	:public IEngineCallbacks
{
public:
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual void debugCallback(const QString &msg)override;
};

#endif // GDILENGINECALLBACKS_H
