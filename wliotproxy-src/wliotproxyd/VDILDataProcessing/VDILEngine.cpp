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

#include "VDILEngine.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/ProgramDevicesBridge.h"
#include "VDIL/core/ProgramStoragesBridge.h"
#include "wliot/devices/VirtualDeviceBackend.h"
#include "../ServerInstance.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

VDILEngine::VDILEngine(IdType uid,BlocksFactory *bf,BlocksXmlParserFactory *xbf,QObject *parent)
	:BaseProgramEngine(parent)
	,helper(uid)
	,callbacks(this)
{
	blocksFact=bf;
	blocksXmlFact=xbf;
	prg=nullptr;
	trd=new VDILProgramThread(&helper,&callbacks,this);
	mVDevBackend=0;
}

VDILEngine::~VDILEngine()
{
	if(trd->isRunning())
		stopAndCleanup();
	else delete trd;

	if(prg)delete prg;
	if(mVDevBackend)
		mVDevBackend->forceDisconnect();
}

void VDILEngine::setProgram(Program *p)
{
	if(prg)
		delete prg;
	prg=p;
	trd->setProgram(prg);
	if(!prg)return;
	connect(prg->vdev(),&ProgramVirtualDevice::activateProgram,
		trd,&VDILProgramThread::activateProgram,Qt::DirectConnection);
	connect(prg->devBr(),&ProgramDevicesBridge::activateProgram,
		trd,&VDILProgramThread::activateProgram,Qt::DirectConnection);
	connect(prg->storBr(),&ProgramStoragesBridge::activateProgram,
		trd,&VDILProgramThread::activateProgram,Qt::DirectConnection);
}

Program* VDILEngine::program()
{
	return prg;
}

void VDILEngine::start()
{
	if(trd->isRunning())
		return;
	if(!prg)
		return;
	prg->prepareToStart();
	trd->start();
	while(!trd->isRunning())
		QThread::yieldCurrentThread();
	trd->setPriority(QThread::LowPriority);
	tmrTrd=new VDILTimersThread(prg,trd,this);
	if(prg->vdev()->enabled()&&!prg->vdev()->devId().isNull())
	{
		if(!mVDevBackend)
		{
			mVDevBackend=new VirtualDeviceBackend(prg->vdev()->devId(),prg->vdev()->devName(),QUuid(),
				static_cast<IVirtualDeviceBackendCallback*>(prg->vdev()),this);
			connect(mVDevBackend,&VirtualDeviceBackend::destroyed,this,&VDILEngine::onVDevBackendDestroyed);
		}
		ServerInstance::inst().devices()->registerVirtualDevice(mVDevBackend);
	}
}

void VDILEngine::stop()
{
	if(!trd->isRunning())return;
	stopAndCleanup();
	trd=new VDILProgramThread(&helper,&callbacks,this);
	trd->setProgram(prg);
	if(mVDevBackend)
		mVDevBackend->forceDisconnect();
}

bool VDILEngine::isRunning()
{
	return trd->isRunning();
}

bool VDILEngine::setData(const QByteArray &data)
{
	if(trd->isRunning())
		return false;
	Program *p=ProgramXmlParser::fromXml(blocksXmlFact,blocksFact,data,false);
	if(!p)return false;
	prg=p;
	trd->setProgram(prg);
	return true;
}

QByteArray VDILEngine::data()
{
	if(!prg)return QByteArray();
	return ProgramXmlParser::toXml(blocksXmlFact,prg);
}

void VDILEngine::onProgramNameChanged()
{
	callbacks.setProgramName(programName());
}

void VDILEngine::onVDevBackendDestroyed()
{
	mVDevBackend=0;
}

void VDILEngine::stopAndCleanup()
{
	tmrTrd->requestInterruption();
	tmrTrd->wait(500);
	tmrTrd->terminate();
	tmrTrd->wait(100);
	trd->requestInterruption();
	trd->wait(300);
	trd->terminate();
	trd->wait(100);
	delete tmrTrd;
	delete trd;
	prg->cleanupAfterStop();
}

void VDILEngine::sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)
{
	mVDevBackend->emulateMessageFromDevice(
		Message(WLIOTProtocolDefs::measurementBMsg,QByteArrayList()<<sensorName<<data));
}
