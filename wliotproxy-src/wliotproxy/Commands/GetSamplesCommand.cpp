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

#include "GetSamplesCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

using namespace WLIOT;
using namespace WLIOTClient;

GetSamplesCommand::GetSamplesCommand(const CmdArgParser &p,ServerConnection *d)
	:IClientCommand(p,d)
{
	tryParseTime=parser.keys.contains("t");
}

bool GetSamplesCommand::evalCommand()
{
	if(parser.args.count()<4)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::getSamplesCommand);
		return false;
	}
	return writeCommandToServer(IClientCommand::getSamplesCommand.toUtf8(),stringListToByteArrayList(parser.args));
}

bool GetSamplesCommand::onCmdData(const QByteArrayList &args)
{
	if(args.count()>0)
	{
		QByteArrayList argsCopy=args;
		if(tryParseTime)
		{
			bool ok=false;
			qint64 tms=argsCopy[0].toLongLong(&ok);
			if(ok)
				argsCopy[0]=QDateTime::fromMSecsSinceEpoch(tms).toString(Qt::ISODate).toUtf8();
		}
		StdQFile::inst().stdoutDebug()<<"Command result: "<<argsCopy.join(WLIOTProtocolDefs::argDelim)<<"\n";
	}
	return true;
}
