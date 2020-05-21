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

#ifndef GDILENGINE_H
#define GDILENGINE_H

#include "../BaseDataProcessing/BaseProgramEngine.h"
#include "GDILProgramThread.h"
#include "GDILTimersThread.h"
#include "GDIL/core/IEngineCallbacks.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDILEngineCallbacks.h"
#include "GDILEngineHelper.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class GDILEngine
	:public BaseProgramEngine
{
	Q_OBJECT
public:
	explicit GDILEngine(IdType uid,BlocksFactory *bf,BlocksXmlParserFactory *xbf,QObject *parent=nullptr);
	virtual ~GDILEngine();
	void setProgram(Program *p);
	Program* program();
	virtual void start()override;
	virtual void stop()override;
	virtual bool isRunning()override;
	virtual bool setData(const QByteArray &data)override;
	virtual QByteArray data()override;

private:
	GDILEngineHelper helper;
	GDILEngineCallbacks cmdCb;
	GDILProgramThread *trd;
	GDILTimersThread *tmrTrd;
	BlocksFactory *blocksFact;
	BlocksXmlParserFactory *blocksXmlFact;
	Program *prg;
};

#endif // GDILENGINE_H
