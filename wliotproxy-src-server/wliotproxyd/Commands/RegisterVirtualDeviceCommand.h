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

#ifndef REGISTERVIRTUALDEVICECOMMAND_H
#define REGISTERVIRTUALDEVICECOMMAND_H

#include "ICommand.h"

class RegisterVirtualDeviceCommand
	:public ICommand
{
public:
	explicit RegisterVirtualDeviceCommand(CommandProcessor *p);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool registerVDev(CallContext &ctx);
	bool disconnectVDev(CallContext &ctx);
};

#endif // REGISTERVIRTUALDEVICECOMMAND_H
