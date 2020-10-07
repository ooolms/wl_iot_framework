#ifndef ENGINE_H
#define ENGINE_H

#include <VDIL/core/Program.h>
#include <wliot/client/ServerInstance.h>
#include <wliot/client/VirtualDeviceCallback.h>
#include "EngineCallbacks.h"
#include "EngineHelper.h"
#include "TimersThread.h"

class Engine
	:public QObject
	,public WLIOTClient::VirtualDeviceCallback
{
	Q_OBJECT
public:
	explicit Engine(WLIOTClient::ServerInstance *s,WLIOTVDIL::Program *p);
	virtual ~Engine();

protected:
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;
	virtual void prepareState(WLIOT::DeviceState &st)override;

private slots:
	void onActivateProgram();

private:
	WLIOTClient::ServerInstance *srv;
	WLIOTVDIL::Program *prg;
	WLIOTClient::VirtualDeviceClient *cli;
	EngineCallbacks cb;
	EngineHelper hlp;
	TimersThread timers;
};

#endif // ENGINE_H
