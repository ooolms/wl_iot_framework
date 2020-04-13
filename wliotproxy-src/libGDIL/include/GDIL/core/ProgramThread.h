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

#ifndef PROGRAMTHREAD_H
#define PROGRAMTHREAD_H

#include <QThread>
#include "GDIL/core/Program.h"
#include "GDIL/core/IEngineHelper.h"
#include "GDIL/core/IEngineCallbacks.h"

class ProgramObject;

//не владеет программой!
class ProgramThread
	:public QThread
{
	Q_OBJECT
public:
	explicit ProgramThread(IEngineHelper *hlp,IEngineCallbacks *ccb,QObject *parent=nullptr);
	virtual ~ProgramThread();
	void setProgram(Program *p);
	void start();

protected:
	virtual void run()override;

private slots:
	void onExecCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args);
	void onDebugMessage(const QString &msg);

private:
	IEngineHelper *helper;
	IEngineCallbacks *cmdCb;
	Program *prg;
	ProgramObject *obj;
};

#endif // PROGRAMTHREAD_H
