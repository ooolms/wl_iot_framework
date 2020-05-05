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

#ifndef GDILCONTROLCOMMAND_H
#define GDILCONTROLCOMMAND_H

#include "ICommand.h"

class GDILProgramsManager;

class GDILControlCommand
	:public ICommand
{
public:
	explicit GDILControlCommand(CommandProcessor *p);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool list(CallContext &ctx);
	bool start(const QByteArray &script,CallContext &ctx);
	bool stop(const QByteArray &script,CallContext &ctx);
	bool restart(const QByteArray &script,CallContext &ctx);
	bool get(const QByteArray &script,CallContext &ctx);
	bool upload(const QByteArray &script,CallContext &ctx);
	bool remove(const QByteArray &script,CallContext &ctx);
	bool listConfigOptions(const QByteArray &script,CallContext &ctx);
	bool setConfigOption(const QByteArray &script,CallContext &ctx);
	bool listTimers(const QByteArray &script,CallContext &ctx);
	bool setTimer(const QByteArray &script,CallContext &ctx);

private:
	GDILProgramsManager *mgr;
};

#endif // GDILCONTROLCOMMAND_H
