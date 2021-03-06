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

#include "VDILProgramThread.h"
#include "VDIL/core/ProgramObject.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include <QDebug>
#include <QAbstractEventDispatcher>

using namespace WLIOTVDIL;
using namespace WLIOT;

VDILProgramThread::VDILProgramThread(IEngineHelper *hlp,IEngineCallbacks *ccb,QObject *parent)
	:QThread(parent)
{
	helper=hlp;
	cmdCb=ccb;
	prg=0;
	obj=new ProgramObject(helper);
}

VDILProgramThread::~VDILProgramThread()
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

//Урезать хелпер, чтобы не выдавать указатель на хранилище (если кто-нибудь его грохнет, все упадет)
void VDILProgramThread::setProgram(Program *p)
{
	if(isRunning())return;
	delete obj;
	prg=p;
	obj=new ProgramObject(helper);
	obj->setProgram(p);
	connect(obj,&ProgramObject::execCommand,this,&VDILProgramThread::onExecCommand,Qt::QueuedConnection);
	connect(obj,&ProgramObject::debugMessage,this,&VDILProgramThread::onDebugMessage,Qt::QueuedConnection);
	connect(obj,&ProgramObject::vdevMeasurement,this,&VDILProgramThread::onVDevMeasurementMessage,Qt::QueuedConnection);
}

void VDILProgramThread::start()
{
	if(isRunning())return;
	if(!prg)return;
	QThread::start();
	while(!isRunning())
		QThread::yieldCurrentThread();
}

void VDILProgramThread::stop()
{
	if(!isRunning())return;
	requestInterruption();
	runSem.release();
}

void VDILProgramThread::activateProgram()
{
	if(!isRunning())return;
	obj->extractSources();
	runSem.release();
}

void VDILProgramThread::run()
{
	while(1)
	{
		if(!runSem.tryAcquire(qMax(runSem.available(),1),200))
			continue;
		if(isInterruptionRequested())
			return;
		else obj->activateProgram();
	}
}

void VDILProgramThread::onExecCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	cmdCb->commandCallback(devId,cmd,args);
}

void VDILProgramThread::onDebugMessage(const QString &msg)
{
	cmdCb->debugCallback(msg);
}

void VDILProgramThread::onVDevMeasurementMessage(const QByteArray &sensorName,const QByteArray &data)
{
	cmdCb->sendVDevMeasurementB(sensorName,data);
}
