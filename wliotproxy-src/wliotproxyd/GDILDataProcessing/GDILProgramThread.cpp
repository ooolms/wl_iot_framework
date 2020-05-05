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

#include "GDILProgramThread.h"
#include "GDIL/core/ProgramObject.h"

GDILProgramThread::GDILProgramThread(IEngineHelper *hlp,IEngineCallbacks *ccb,QObject *parent)
	:QThread(parent)
{
	helper=hlp;
	cmdCb=ccb;
	prg=0;
	obj=new ProgramObject(helper);
}

GDILProgramThread::~GDILProgramThread()
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

void GDILProgramThread::setProgram(Program *p)
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
	connect(obj,&ProgramObject::execCommand,this,&GDILProgramThread::onExecCommand,Qt::QueuedConnection);
	connect(obj,&ProgramObject::debugMessage,this,&GDILProgramThread::onDebugMessage,Qt::QueuedConnection);
}

void GDILProgramThread::start()
{
	if(isRunning())return;
	QThread::start();
	while(!isRunning())
		QThread::yieldCurrentThread();
	obj->moveToThread(this);
}

void GDILProgramThread::activateProgram()
{
	QMetaObject::invokeMethod(obj,"activateProgram",Qt::QueuedConnection);
}

void GDILProgramThread::run()
{
	QThread::exec();
	obj->moveToThread(thread());
}

void GDILProgramThread::onExecCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	cmdCb->commandCallback(devId,cmd,args);
}

void GDILProgramThread::onDebugMessage(const QString &msg)
{
	cmdCb->debugCallback(msg);
}
