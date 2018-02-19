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

#ifndef SESSIONSTORAGECOMMANDS_H
#define SESSIONSTORAGECOMMANDS_H

#include "ICommand.h"

class ARpcSessionStorage;

class SessionStorageCommands
	:public ICommand
{
public:
	explicit SessionStorageCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p);

public:
	virtual bool processCommand(const ARpcMessage &m,QByteArrayList &retVal)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool listSessions(const ARpcMessage &m,QByteArrayList &retVal);
	bool listSessionAttrs(const ARpcMessage &m,QByteArrayList &retVal);
	bool getSessionAttr(const ARpcMessage &m,QByteArrayList &retVal);
	bool setSessionAttr(const ARpcMessage &m,QByteArrayList &retVal);
	bool sessionStart(const ARpcMessage &m,QByteArrayList &retVal);
	bool sessionStop(const ARpcMessage &m,QByteArrayList &retVal);
	bool getMainWriteSessionId(const ARpcMessage &m,QByteArrayList &retVal);

	ARpcSessionStorage* openStorage(const ARpcMessage &m,QByteArrayList &retVal);
	bool openSession(ARpcSessionStorage *stor,const QUuid &sessionId,QByteArrayList &retVal,bool &closeLater);
};

#endif // SESSIONSTORAGECOMMANDS_H
