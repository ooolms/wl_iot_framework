#ifndef ENGINERUN_H
#define ENGINERUN_H

#include <VDIL/core/Program.h>
#include <wliot/client/ServerInstance.h>
#include <wliot/client/VirtualDeviceCallback.h>
#include <VDIL/core/Engine.h>
#include "EngineCallbacks.h"
#include "EngineHelper.h"
#include "TimersThread.h"

class EngineRun
	:public QObject
	,public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit EngineRun(WLIOTClient::ServerInstance *s);
	bool setup(const QByteArray &data);
	void start();
	WLIOTVDIL::Program* program();
	virtual ~EngineRun();
	void additionalStateChangedForCallback(const QByteArray &paramName,const QByteArray &value);

protected:
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;
	virtual void prepareState(WLIOT::DeviceState &st)override;

private slots:
	void onActivateProgram();

private:
	WLIOTVDIL::Engine engine;
	WLIOTClient::ServerInstance *srv;
	WLIOTVDIL::Program *prg;
	WLIOTClient::VirtualDeviceClient *cli;
	EngineCallbacks cb;
	EngineHelper hlp;
	TimersThread *timers;
};

#endif // ENGINERUN_H
