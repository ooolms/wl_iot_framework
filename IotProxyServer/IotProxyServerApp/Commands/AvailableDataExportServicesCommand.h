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

#ifndef AVAILABLEDATAEXPORTSERVICESCOMMAND_H
#define AVAILABLEDATAEXPORTSERVICESCOMMAND_H

#include "ICommand.h"

class AvailableDataExportServicesCommand
	:public ICommand
{
public:
	explicit AvailableDataExportServicesCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override;
	virtual QByteArrayList acceptedCommands()override;
};

#endif // AVAILABLEDATAEXPORTSERVICESCOMMAND_H
