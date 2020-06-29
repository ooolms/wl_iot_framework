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

using namespace WLIOTVDIL;

VDILEngine::VDILEngine(IdType uid,BlocksFactory *bf,BlocksXmlParserFactory *xbf,QObject *parent)
	:BaseProgramEngine(parent)
	,helper(uid)
{
	blocksFact=bf;
	blocksXmlFact=xbf;
	prg=nullptr;
	trd=new VDILProgramThread(&helper,&cmdCb,this);
}

VDILEngine::~VDILEngine()
{
	if(trd->isRunning())
	{
		tmrTrd->requestInterruption();
		tmrTrd->wait(500);
		tmrTrd->terminate();
		tmrTrd->wait(100);
		delete tmrTrd;

		trd->requestInterruption();
		trd->wait(300);
		trd->terminate();
		trd->wait(100);
	}
	delete trd;
	if(prg)delete prg;
}

void VDILEngine::setProgram(Program *p)
{
	if(prg)
		delete prg;
	prg=p;
	trd->setProgram(prg);
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
	trd->start();
	while(!trd->isRunning())
		QThread::yieldCurrentThread();
	trd->setPriority(QThread::LowPriority);
	tmrTrd=new VDILTimersThread(prg,trd,this);
}

void VDILEngine::stop()
{
	if(!trd->isRunning())return;
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
	trd=new VDILProgramThread(&helper,&cmdCb,this);
	trd->setProgram(prg);
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
