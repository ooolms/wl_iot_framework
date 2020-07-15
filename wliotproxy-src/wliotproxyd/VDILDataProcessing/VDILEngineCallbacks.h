#ifndef VDILENGINECALLBACKS_H
#define VDILENGINECALLBACKS_H

#include "VDIL/core/IEngineCallbacks.h"

class VDILEngine;

class VDILEngineCallbacks
	:public WLIOTVDIL::IEngineCallbacks
{
public:
	explicit VDILEngineCallbacks(VDILEngine *e);
	void setProgramName(const QByteArray &name);
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual void debugCallback(const QString &msg)override;
	virtual void sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)override;

private:
	QByteArray mProgramName;
	VDILEngine *mEngine;
};

#endif // VDILENGINECALLBACKS_H
