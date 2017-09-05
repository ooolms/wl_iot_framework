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

#ifndef EXECDEVICECOMMANDCOMMAND_H
#define EXECDEVICECOMMANDCOMMAND_H

#include "ICommand.h"

class ExecDeviceCommandCommand
	:public ICommand
{
public:
	explicit ExecDeviceCommandCommand(ARpcOutsideDevice *d);

public:
	virtual bool processCommand(const ARpcMessage &m,QStringList &retVal)override;
	virtual QStringList acceptedCommands()override;
};

#endif // EXECDEVICECOMMANDCOMMAND_H
