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

#ifndef BASEPROGRAMSCONTROLCOMMAND_H
#define BASEPROGRAMSCONTROLCOMMAND_H

#include "ICommand.h"
#include "../Processing/BaseProgramsManager.h"

class BaseProgramsControlCommand
	:public ICommand
{
public:
	explicit BaseProgramsControlCommand(const QByteArray &cmdPrefix,BaseProgramsManager *mgr,CommandProcessor *p);
	virtual bool processCommand(CallContext &ctx)override;
	virtual QByteArrayList acceptedCommands()override;

protected:
	static QByteArray noProgramFoundError(const QByteArray &programId);

protected:
	BaseProgramsManager *mMgr;

private:
	QByteArray mCmdPrefix;
};

#endif // BASEPROGRAMSCONTROLCOMMAND_H
