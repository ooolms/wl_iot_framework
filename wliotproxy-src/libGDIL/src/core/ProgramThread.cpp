/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "GDIL/core/ProgramThread.h"
#include "GDIL/core/ProgramObject.h"

ProgramThread::ProgramThread(IEngineHelper *hlp,IEngineCallbacks *ccb,QObject *parent)
	:QThread(parent)
{
	helper=hlp;
	cmdCb=ccb;
	prg=0;
	obj=new ProgramObject(helper);
}

ProgramThread::~ProgramThread()
{
	if(isRunning())
	{
		quit();
		wait(200);
		terminate();
		wait(100);
	}
	delete obj;
}

void ProgramThread::setProgram(Program *p)
{
	if(isRunning())return;
	delete obj;
	prg=p;
	obj=new ProgramObject(helper);
	obj->setProgram(p);
	for(int i=0;i<prg->deviceTriggers().count();++i)
	{
		RealDevice *dev=helper->devById(prg->deviceTriggers()[i]);
		if(!dev)continue;
		connect(dev,&RealDevice::stateChanged,obj,&ProgramObject::activateProgram,Qt::QueuedConnection);
	}
	for(int i=0;i<prg->storageTriggers().count();++i)
	{
		ISensorStorage *stor=helper->storageById(prg->storageTriggers()[i]);
		if(!stor)continue;
		connect(stor,&ISensorStorage::newValueWritten,obj,&ProgramObject::activateProgram,Qt::QueuedConnection);
	}
	connect(obj,&ProgramObject::execCommand,this,&ProgramThread::onExecCommand,Qt::QueuedConnection);
	connect(obj,&ProgramObject::debugMessage,this,&ProgramThread::onDebugMessage,Qt::QueuedConnection);
}

void ProgramThread::start()
{
	if(isRunning())return;
	QThread::start();
	while(!isRunning())
		QThread::yieldCurrentThread();
	obj->moveToThread(this);
}

void ProgramThread::run()
{
	QThread::exec();
	obj->moveToThread(thread());
}

void ProgramThread::onExecCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	cmdCb->commandCallback(devId,cmd,args);
}

void ProgramThread::onDebugMessage(const QString &msg)
{
	cmdCb->debugCallback(msg);
}
