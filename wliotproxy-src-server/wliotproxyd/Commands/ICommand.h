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

#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "wliot/devices/QtIODeviceWrap.h"

class CommandProcessor;

class ICommand
{
public:
	struct CallContext
	{
		QByteArray cmd;
		QByteArray callId;
		QByteArrayList args;
		QByteArrayList retVal;
	};

public:
	explicit ICommand(CommandProcessor *p);
	virtual ~ICommand()
	{
	}

	virtual bool processCommand(CallContext &ctx)=0;//ctx можно испортить
	virtual QByteArrayList acceptedCommands()=0;

protected:
	void writeCmdataMsg(const QByteArray &callId,const QByteArrayList &args);

protected:
	CommandProcessor *proc;
};

#endif // ICOMMAND_H
