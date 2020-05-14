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

#ifndef PROGRAMOBJECT_H
#define PROGRAMOBJECT_H

#include "GDIL/core/Program.h"
#include "GDIL/core/IEngineHelper.h"
#include "GDIL/core/IEngineCallbacks.h"
#include <QObject>

class ProgramObject
	:public QObject
	,public IEngineCallbacks
{
	Q_OBJECT
public:
	explicit ProgramObject(IEngineHelper *hlp,QObject *parent=0);
	virtual ~ProgramObject();
	void setHelper(IEngineHelper *hlp);
	void setProgram(Program *p);
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;
	virtual void debugCallback(const QString &msg)override;
	void extractSources();
	void activateProgram();

signals:
	void execCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args);
	void debugMessage(const QString &m);

private:
	IEngineHelper *helper;
	Program *prg;
};

#endif // PROGRAMOBJECT_H
