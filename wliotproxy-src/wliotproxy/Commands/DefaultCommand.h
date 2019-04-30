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

#ifndef DEFAULTCOMMAND_H
#define DEFAULTCOMMAND_H

#include "../IClientCommand.h"

class DefaultCommand
	:public IClientCommand
{
public:
	explicit DefaultCommand(const CmdArgParser &p,IotServerConnection *c,const QByteArray &cmd,int minArgsCount);
	virtual bool evalCommand()override;

private:
	QByteArray command;
	int minArgs;
};

#endif // DEFAULTCOMMAND_H
