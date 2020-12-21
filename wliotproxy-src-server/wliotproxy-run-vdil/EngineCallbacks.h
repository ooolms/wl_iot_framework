#ifndef ENGINECALLBACKS_H
#define ENGINECALLBACKS_H

#include <VDIL/core/IEngineCallbacks.h>
#include <wliot/client/VirtualDeviceClient.h>
#include <wliot/client/ServerInstance.h>

class EngineRun;

class EngineCallbacks
	:public WLIOTVDIL::IEngineCallbacks
{
public:
	explicit EngineCallbacks(WLIOTClient::ServerInstance *s,EngineRun *e);
	void setProgramName(const QByteArray &name);
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual bool commandCallbackWaitAnswer(const QUuid &devId,const QByteArray &cmd,
		const QByteArrayList &args,QByteArrayList &retVal)override;
	virtual void debugCallback(const QString &msg)override;
	virtual void sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)override;
	virtual void vdevAdditionalStateChanged(const QByteArray &paramName,const QByteArray &value)override;

private:
	QByteArray mProgramName;
	WLIOTClient::ServerInstance *srv;
	WLIOTClient::VirtualDeviceClient *vDev;
	EngineRun *eng;
};

#endif // ENGINECALLBACKS_H
