#include "GIDL/core/Engine.h"

//TODO add QObject wrap for a Program to use in a thread

Engine::Engine(QObject *parent)
	:QObject(parent)
{
	prg=nullptr;
	trd=new QThread(this);
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

void Engine::start(Program *p)
{
	if(trd->isRunning())return;
	prg=p;
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
	trd=new QThread(this);
}
