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
	prg->setEngineCallbacks(0);
}

void ProgramObject::setProgram(Program *p)
{
	prg=p;
	if(!prg)return;
	prg->setHelper(helper);
	prg->setEngineCallbacks(static_cast<IEngineCallbacks*>(this));
}

void ProgramObject::commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)
{
	emit execCommand(devId,cmd,args);
}

void ProgramObject::debugCallback(const QString &msg)
{
	emit debugMessage(msg);
}

void ProgramObject::activateProgram()
{
	prg->extractSources();
	prg->prepareWorkData();
	prg->eval();
}
