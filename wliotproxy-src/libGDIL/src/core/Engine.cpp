#include "GDIL/core/Engine.h"

//TODO add QObject wrap for a Program to use in a thread

Engine::Engine(IEngineHelper *hlp,QObject *parent)
	:QObject(parent)
{
	helper=hlp;
	prg=nullptr;
	trd=new ProgramThread(helper,this);
}

Engine::~Engine()
{
	if(trd->isRunning())
	{
		trd->quit();
		trd->wait(300);
		trd->terminate();
		trd->wait(100);
	}
	delete trd;
	if(prg)delete prg;
}

void Engine::setProgram(Program *p)
{
	if(trd->isRunning())return;
	if(prg)delete prg;
	prg=p;
	trd->setProgram(prg);
}

void Engine::start()
{
	if(trd->isRunning())return;
	if(!prg)return;
	trd->start();
	while(!trd->isRunning())
		QThread::yieldCurrentThread();
}

void Engine::stop()
{
	if(!trd->isRunning())return;
	trd->quit();
	trd->wait(300);
	trd->terminate();
	trd->wait(100);
	delete trd;
	trd=new ProgramThread(helper,this);
	trd->setProgram(prg);
}
