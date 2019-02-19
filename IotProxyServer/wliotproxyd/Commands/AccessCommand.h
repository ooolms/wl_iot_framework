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

#ifndef ACCESSCOMMAND_H
#define ACCESSCOMMAND_H

#include "ICommand.h"

class AccessMgr;

class AccessCommand
	:public ICommand
{
public:
	explicit AccessCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool processUserCommand(CallContext &ctx);
	bool processUserGroupCommand(CallContext &ctx);
	bool processDevCommand(CallContext &ctx);

private:
	AccessMgr &accessMgr;
};

#endif // ACCESSCOMMAND_H
