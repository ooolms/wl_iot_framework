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

#include "ARpcBase/ARpcOutsideDevice.h"

class IotProxyCommandProcessor;

class ICommand
{
public:
	explicit ICommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p);
	virtual ~ICommand()
	{
	}

	bool processCommand(const QByteArray &cmd,const QByteArray &cId,const QByteArrayList &args,QByteArrayList &retVal);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)=0;
	virtual QByteArrayList acceptedCommands()=0;

protected:
	void writeCmdataMsg(const QByteArrayList &args);

protected:
	ARpcOutsideDevice *clientDev;
	IotProxyCommandProcessor *proc;
	QByteArray callId;
};

#endif // ICOMMAND_H
