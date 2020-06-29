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

#ifndef VDILENGINE_H
#define VDILENGINE_H

#include "../BaseDataProcessing/BaseProgramEngine.h"
#include "VDILProgramThread.h"
#include "VDILTimersThread.h"
#include "VDIL/core/IEngineCallbacks.h"
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDILEngineCallbacks.h"
#include "VDILEngineHelper.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class VDILEngine
	:public BaseProgramEngine
{
	Q_OBJECT
public:
	explicit VDILEngine(IdType uid,WLIOTVDIL::BlocksFactory *bf,
		WLIOTVDIL::BlocksXmlParserFactory *xbf,QObject *parent=nullptr);
	virtual ~VDILEngine();
	void setProgram(WLIOTVDIL::Program *p);
	WLIOTVDIL::Program* program();
	virtual void start()override;
	virtual void stop()override;
	virtual bool isRunning()override;
	virtual bool setData(const QByteArray &data)override;
	virtual QByteArray data()override;

protected:
	virtual void onProgramNameChanged()override;

private:
	VDILEngineHelper helper;
	VDILEngineCallbacks callbacks;
	VDILProgramThread *trd;
	VDILTimersThread *tmrTrd;
	WLIOTVDIL::BlocksFactory *blocksFact;
	WLIOTVDIL::BlocksXmlParserFactory *blocksXmlFact;
	WLIOTVDIL::Program *prg;
};

#endif // VDILENGINE_H
