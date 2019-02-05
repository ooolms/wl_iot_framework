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

#ifndef STORAGESCOMMANDS_H
#define STORAGESCOMMANDS_H

#include "ICommand.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"

class StoragesCommands
	:public ICommand
{
public:
	explicit StoragesCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p);
	static QByteArrayList storageToMsgArguments(ARpcISensorStorage *s);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool listStorages(CallContext &ctx);
	bool addStorage(CallContext &ctx);
	bool removeStorage(CallContext &ctx);
	bool addDataExport(CallContext &ctx);
	bool getDataExport(CallContext &ctx);
	bool allDataExports(CallContext &ctx);
	bool getAttr(CallContext &ctx);
	bool setAttr(CallContext &ctx);
};

#endif // STORAGESCOMMANDS_H
