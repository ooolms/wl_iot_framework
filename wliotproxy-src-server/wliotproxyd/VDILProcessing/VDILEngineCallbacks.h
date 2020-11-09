#ifndef VDILENGINECALLBACKS_H
#define VDILENGINECALLBACKS_H

#include "VDIL/core/IEngineCallbacks.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class VDILEngine;

class VDILEngineCallbacks
	:public WLIOTVDIL::IEngineCallbacks
{
public:
	explicit VDILEngineCallbacks(IdType uid,VDILEngine *e);
	void setProgramName(const QByteArray &name);
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual bool commandCallbackWaitAnswer(const QUuid &devId,const QByteArray &cmd,
		const QByteArrayList &args,QByteArrayList &retVal)override;
	virtual void debugCallback(const QString &msg)override;
	virtual void sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)override;
//	virtual void vdevAdditionalStateChanged(const QByteArray &paramName,const QByteArray &value)override;

private:
	QByteArray mProgramName;
	VDILEngine *mEngine;
	IdType mUid;
};

#endif // VDILENGINECALLBACKS_H
