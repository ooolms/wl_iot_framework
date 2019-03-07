/*******************************************
Copyright 2018 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef HELPERCOMMAND_H
#define HELPERCOMMAND_H

#include "../IClientCommand.h"
#include <functional>

class HelperCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit HelperCommand(const CmdArgParser &p,IotServerConnection *c);
	virtual bool evalCommand()override;
	Q_INVOKABLE void done();

protected:
	virtual bool onCmdData(const QByteArrayList &args);
	virtual bool onOk(const QByteArrayList &args);

private:
	std::function<void(const QByteArrayList &args)> onCmdDataFunc;
	QByteArrayList wordList;
};

#endif // HELPERCOMMAND_H
