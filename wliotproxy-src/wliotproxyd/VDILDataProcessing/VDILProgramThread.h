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

#ifndef VDILPROGRAMTHREAD_H
#define VDILPROGRAMTHREAD_H

#include <QThread>
#include <QSemaphore>
#include "VDIL/core/Program.h"
#include "VDIL/core/IEngineHelper.h"
#include "VDIL/core/IEngineCallbacks.h"

namespace WLIOTVDIL
{
	class ProgramObject;
}

//не владеет программой!
class VDILProgramThread
	:public QThread
{
	Q_OBJECT
public:
	explicit VDILProgramThread(WLIOTVDIL::IEngineHelper *hlp,WLIOTVDIL::IEngineCallbacks *ccb,QObject *parent=nullptr);
	virtual ~VDILProgramThread();
	void setProgram(WLIOTVDIL::Program *p);
	void start();

public slots:
	void activateProgram();

protected:
	virtual void run()override;

private slots:
	void onExecCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args);
	void onDebugMessage(const QString &msg);

private:
	WLIOTVDIL::IEngineHelper *helper;
	WLIOTVDIL::IEngineCallbacks *cmdCb;
	WLIOTVDIL::Program *prg;
	WLIOTVDIL::ProgramObject *obj;
	QSemaphore runSem;
};

#endif // VDILPROGRAMTHREAD_H
