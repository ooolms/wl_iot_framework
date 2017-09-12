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

#ifndef DEVICESCONFIGCOMMAND_H
#define DEVICESCONFIGCOMMAND_H

#include "../IClientCommand.h"

class DevicesConfigCommand
	:public IClientCommand
{
public:
	explicit DevicesConfigCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

protected:
	virtual bool onOk(const QStringList &args)override;

private:
	QString subCommand;
};

#endif // DEVICESCONFIGCOMMAND_H