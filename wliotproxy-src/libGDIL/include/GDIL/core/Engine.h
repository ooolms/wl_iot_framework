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

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "GDIL/core/ProgramThread.h"
#include "GDIL/core/IEngineCallbacks.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"

class Engine
	:public QObject
{
	Q_OBJECT
public:
	explicit Engine(IEngineHelper *hlp,IEngineCallbacks *ccb,
		BlocksFactory *bf,BlocksXmlParserFactory *xbf,QObject *parent=nullptr);
	virtual ~Engine();
	void setProgram(Program *p);
	bool setProgram(const QByteArray &xmlData);
	void start();
	void stop();
	bool isRunning();

private:
	IEngineHelper *helper;
	IEngineCallbacks *cmdCb;
	ProgramThread *trd;
	BlocksFactory *blocksFact;
	BlocksXmlParserFactory *blocksXmlFact;
	Program *prg;
};

#endif // ENGINE_H
