#include "EngineRun.h"
#include <VDIL/core/ProgramVirtualDevice.h>
#include <VDIL/xml/ProgramXmlParser.h>
#include <VDIL/CoreBlocksPlugin.h>
#include <VDIL/core/ProgramDevicesRuntime.h>
#include <VDIL/core/ProgramStoragesRuntime.h>
#include <QDebug>
#include "MainServerConfig.h"

using namespace WLIOTVDIL;

//CRIT переделать activate сигналы, чтобы они передавали список id блоков,
//для которых сработала активация, и у которых надо выставить флаг trigger

EngineRun::EngineRun(WLIOTClient::ServerInstance *s)
	:cb(s,this)
	,hlp(s)
{
	srv=s;
	prg=0;
	cli=0;
	timers=0;
	CoreBlocksPlugin *p=new CoreBlocksPlugin;
	if(!engine.plg.registerPlugin(p))
		delete p;
	engine.plg.loadPlugins(MainServerConfig::vdilPluginsPath);
}

bool EngineRun::setup(const QByteArray &data)
{
	prg=ProgramXmlParser::fromXml(&engine,data,false);
	if(!prg)return false;
	prg->setHelper(&hlp);
	prg->setEngineCallbacks(&cb);
	for(auto r:prg->runtimes())
		connect(r,&IProgramRuntimeInstance::activateProgram,this,&EngineRun::onActivateProgram,Qt::QueuedConnection);
	prg->prepareToStart();
	timers=new TimersThread(prg);
	connect(timers,&TimersThread::activate,this,&EngineRun::onActivateProgram);
	return true;
}

void EngineRun::start()
{
	if(!prg)return;
	if(!prg->vdev()->devId().isNull())
	{
		cli=srv->devices()->registerVirtualDevice(prg->vdev()->devId(),prg->vdev()->devName(),
			prg->vdev()->sensors(),prg->vdev()->controls(),prg->vdev()->typeId(),
			static_cast<VirtualDeviceCallback*>(this));
	}
	timers->start();
	qDebug()<<"Starting program";
}

Program* EngineRun::program()
{
	return prg;
}

EngineRun::~EngineRun()
{
	if(!prg)return;
	timers->requestInterruption();
	timers->wait(500);
	timers->terminate();
	timers->wait(100);
	delete timers;
	prg->cleanupAfterStop();
	delete prg;
}

bool EngineRun::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(!prg||!cli)return false;
	bool ok=prg->vdev()->onCommand(cmd,args,retVal);
	if(ok)
	{
		auto oldSt=cli->state().commandParams.value(cmd);
		auto newSt=prg->vdev()->state().commandParams.value(cmd);
		for(auto i=newSt.begin();i!=newSt.end();++i)
		{
			if(oldSt[i.key()]!=i.value())
				if(cli)cli->commandParamStateChanged(cmd,i.key(),i.value());
		}
	}
	return ok;
}

void EngineRun::prepareState(WLIOT::DeviceState &st)
{
	st=prg->vdev()->state();
}

void EngineRun::onActivateProgram()
{
	qDebug()<<"Activating program";
	prg->extractSources();
	prg->prepareWorkData();
	prg->eval();
	prg->cleanupAfterEval();
}
