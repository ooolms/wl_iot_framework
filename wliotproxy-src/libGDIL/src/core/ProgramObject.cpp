#include "GDIL/core/ProgramObject.h"

ProgramObject::ProgramObject(IEngineHelper *hlp,QObject *parent)
	:QObject(parent)
{
	helper=hlp;
	prg=0;
}

ProgramObject::~ProgramObject()
{
	if(!prg)return;
	prg->setHelper(0);
	prg->setCommandCallback(0);
}

void ProgramObject::setProgram(Program *p)
{
	prg=p;
	if(!prg)return;
}

void ProgramObject::prepare()
{
	if(!prg)return;
	prg->setHelper(helper);
	prg->setCommandCallback(static_cast<ICommandCallback*>(this));
	for(int i=0;i<prg->deviceTriggers.count();++i)
	{
		RealDevice *dev=helper->devById(prg->deviceTriggers[i]);
		if(!dev)continue;
		connect(dev,&RealDevice::stateChanged,this,&ProgramObject::activateProgram,Qt::QueuedConnection);
	}
	for(int i=0;i<prg->storageTriggers.count();++i)
	{
		ISensorStorage *stor=helper->storageById(prg->storageTriggers[i]);
		if(!stor)continue;
		connect(stor,&ISensorStorage::newValueWritten,this,&ProgramObject::activateProgram,Qt::QueuedConnection);
	}
}

void ProgramObject::activateProgram()
{
	prg->extractSources();
	prg->prepareWorkData();
	prg->eval();
}
