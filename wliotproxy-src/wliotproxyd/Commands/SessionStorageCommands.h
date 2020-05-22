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

namespace WLIOT
{
	class SessionStorage;
}

class SessionStorageCommands
	:public ICommand
{
public:
	explicit SessionStorageCommands(CommandProcessor *p);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool listSessions(CallContext &ctx);
	bool listSessionAttrs(CallContext &ctx);
	bool getSessionAttr(CallContext &ctx);
	bool setSessionAttr(CallContext &ctx);
	bool sessionCreate(CallContext &ctx);
	bool sessionStart(CallContext &ctx);
	bool sessionStop(CallContext &ctx);
	bool sessionContinue(CallContext &ctx);
	bool sessionRemove(CallContext &ctx);
	bool getMainWriteSessionId(CallContext &ctx);

	WLIOT::SessionStorage* openStorage(CallContext &ctx);
	bool openSession(WLIOT::SessionStorage *stor,const QUuid &sessionId,QByteArrayList &retVal,bool &closeLater);
};

#endif // SESSIONSTORAGECOMMANDS_H
