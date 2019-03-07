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

#ifndef GETSAMPLESCOMMAND_H
#define GETSAMPLESCOMMAND_H

#include "ICommand.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/ISensorStorage.h"

class GetSamplesCommand
	:public ICommand
{
public:
	explicit GetSamplesCommand(QtIODeviceWrap *d,IotProxyCommandProcessor *p);

public:
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

private:
	bool getSamples(CallContext &ctx,int firstIndexArgument,ISensorStorage *st);
	bool getSamplesBin(CallContext &ctx,int firstIndexArgument,ISensorStorage *st);
};

#endif // GETSAMPLESCOMMAND_H
