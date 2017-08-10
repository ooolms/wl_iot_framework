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

#ifndef LISTIDENTIFIEDCOMMAND_H
#define LISTIDENTIFIEDCOMMAND_H

#include "../IClientCommand.h"

class ListIdentifiedCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ListIdentifiedCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // LISTIDENTIFIEDCOMMAND_H
