#include "Engine.h"
#include <VDIL/core/ProgramDevicesBridge.h>
#include <VDIL/core/ProgramStoragesBridge.h>
#include <VDIL/core/ProgramVirtualDevice.h>
#include <QDebug>

using namespace WLIOTVDIL;

//CRIT переделать activate сигналы, чтобы они передавали список id блоков,
//для которых сработала активация, и у которых надо выставить флаг trigger

Engine::Engine(WLIOTClient::ServerInstance *s,WLIOTVDIL::Program *p)
	:cb(s)
	,hlp(s)
	,timers(p,this)
{
	srv=s;
	prg=p;
	cli=0;
	prg->setHelper(&hlp);
	prg->setEngineCallbacks(&cb);
	if(!prg->vdev()->devId().isNull())
	{
		cli=srv->devices()->registerVirtualDevice(prg->vdev()->devId(),prg->vdev()->devName(),
			prg->vdev()->sensors(),prg->vdev()->controls(),prg->vdev()->typeId(),
			static_cast<VirtualDeviceCallback*>(this));
	}
	connect(prg->vdev(),&ProgramVirtualDevice::activateProgram,
		this,&Engine::onActivateProgram,Qt::QueuedConnection);
	connect(prg->devBr(),&ProgramDevicesBridge::activateProgram,
		this,&Engine::onActivateProgram,Qt::QueuedConnection);
	connect(prg->storBr(),&ProgramStoragesBridge::activateProgram,
		this,&Engine::onActivateProgram,Qt::QueuedConnection);
	connect(&timers,&TimersThread::activate,
		this,&Engine::onActivateProgram,Qt::QueuedConnection);
	prg->prepareToStart();
	timers.start();
	qDebug()<<"Starting program";
}

Engine::~Engine()
{
	timers.requestInterruption();
	timers.wait(500);
	timers.terminate();
	timers.wait(100);
	prg->cleanupAfterStop();
}

bool Engine::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(!cli)return false;
	bool ok=prg->vdev()->onCommand(cmd,args,retVal);
	if(ok)
	{
		auto oldSt=cli->state().commandParams.value(cmd);
		auto newSt=prg->vdev()->state().commandParams.value(cmd);
		for(auto i=newSt.begin();i!=newSt.end();++i)
		{
			if(oldSt[i.key()]!=i.value())
				commandParamStateChanged(cmd,i.key(),i.value());
		}
	}
	return ok;
}

void Engine::prepareState(WLIOT::DeviceState &st)
{
	st=prg->vdev()->state();
}

void Engine::onActivateProgram()
{
	qDebug()<<"Activating program";
	prg->extractSources();
	prg->prepareWorkData();
	prg->eval();
	prg->cleanupAfterEval();
}
