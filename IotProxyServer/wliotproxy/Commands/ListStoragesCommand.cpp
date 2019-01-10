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

#include "ListStoragesCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "ARpcBase/ARpcServerConfig.h"
#include <QDebug>

ListStoragesCommand::ListStoragesCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool ListStoragesCommand::evalCommand()
{
	return writeCommandToServer(IClientCommand::listStoragesCommand);
}

bool ListStoragesCommand::onCmdData(const QByteArrayList &args)
{
	if(args.count()<8)return false;
	QByteArrayList constraintsStrs=args[4].split(';');
	constraintsStrs.removeAll(QByteArray());
	QMap<QByteArray,QByteArray> constraints;
	for(QByteArray &s:constraintsStrs)
	{
		int index=s.indexOf('=');
		if(index==-1)continue;
		constraints[s.mid(0,index)]=s.mid(index+1);
	}
	QDebug d=StdQFile::inst().stdoutDebug();
	d<<"Storage:";
	d<<"\n\tdevice id: "<<args[0];
	d<<"\n\tdevice name: "<<args[1];
	d<<"\n\tsensor name: "<<args[2];
	d<<"\n\tsensor values type: "<<args[3];
	d<<"\n\tconstraints:";//args[4]
	for(auto i=constraints.begin();i!=constraints.end();++i)
		d<<"\n\t\t"<<i.key()<<" = "<<i.value();
	d<<"\n\tstorage type: "<<args[5];
	d<<"\n\ttimestamp transformation rule: "<<args[6];
	d<<"\n\tstored values type: "<<args[7]<<"\n";
	return true;
}
