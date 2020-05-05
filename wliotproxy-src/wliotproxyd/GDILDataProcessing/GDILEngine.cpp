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

#include "GDILEngine.h"
#include "GDIL/xml/ProgramXmlParser.h"

GDILEngine::GDILEngine(IEngineHelper *hlp,IEngineCallbacks *ccb,
	BlocksFactory *bf,BlocksXmlParserFactory *xbf,QObject *parent)
	:QObject(parent)
{
	helper=hlp;
	cmdCb=ccb;
	blocksFact=bf;
	blocksXmlFact=xbf;
	prg=nullptr;
	trd=new GDILProgramThread(helper,cmdCb,this);
}

GDILEngine::~GDILEngine()
{
	if(trd->isRunning())
	{
		tmrTrd->requestInterruption();
		tmrTrd->wait(500);
		tmrTrd->terminate();
		tmrTrd->wait(100);
		delete tmrTrd;

		trd->quit();
		trd->wait(300);
		trd->terminate();
		trd->wait(100);
	}
	delete trd;
	if(prg)delete prg;
}

void GDILEngine::setProgram(Program *p)
{
	if(trd->isRunning())return;
	if(prg)
		delete prg;
	prg=p;
	trd->setProgram(prg);
}

bool GDILEngine::setProgram(const QByteArray &xmlData)
{
	if(trd->isRunning())
		return false;
	Program *p=ProgramXmlParser::fromXml(blocksXmlFact,blocksFact,xmlData);
	if(!p)return false;
	prg=p;
	trd->setProgram(prg);
	return true;
}

Program* GDILEngine::program()
{
	return prg;
}

void GDILEngine::start()
{
	if(trd->isRunning())
		return;
	if(!prg)
		return;
	trd->start();
	while(!trd->isRunning())
		QThread::yieldCurrentThread();
	tmrTrd=new GDILTimersThread(prg,trd,this);
}

void GDILEngine::stop()
{
	if(!trd->isRunning())return;
	tmrTrd->requestInterruption();
	tmrTrd->wait(500);
	tmrTrd->terminate();
	tmrTrd->wait(100);
	trd->quit();
	trd->wait(300);
	trd->terminate();
	trd->wait(100);
	delete tmrTrd;
	delete trd;
	trd=new GDILProgramThread(helper,cmdCb,this);
	trd->setProgram(prg);
}

bool GDILEngine::isRunning()
{
	return trd->isRunning();
}
