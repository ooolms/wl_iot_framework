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
#include "../IotProxyInstance.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "ARpcBase/ARpcServerConfig.h"
#include "StandardErrors.h"
#include "ARpcLocalStorage/ARpcAllStorages.h"
#include "ARpcBase/ARpcAllSensorValues.h"

GetSamplesCommand::GetSamplesCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool GetSamplesCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId(m.args[0]);
	QString sensorName(m.args[1]);
	ARpcISensorStorage *st=IotProxyInstance::inst().sensorsStorage()->existingStorage({deviceId,sensorName});
	if(!st)
	{
		retVal.append("no storage found");
		return false;
	}
	QUuid sessionId;
	int firstIndexArgument=2;
	if(st->getStoreMode()==ARpcISensorStorage::MANUAL_SESSIONS||st->getStoreMode()==ARpcISensorStorage::AUTO_SESSIONS)
	{
		firstIndexArgument=3;
		if(m.args.count()<3)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		sessionId=QUuid(m.args[2]);
		ARpcSessionStorage *sStor=(ARpcSessionStorage*)st;
		if(sessionId.isNull()||!sStor->setMainReadSessionId(sessionId))
		{
			retVal.append("can't open session: "+sessionId.toString());
			return false;
		}
	}
	if(m.title=="get_samples_count")
	{
		retVal.append(QString::number(st->valuesCount()));
		return true;
	}
	else if(m.title=="get_samples")
	{
		quint64 sIndex,eIndex;
		if(m.args.count()<(firstIndexArgument+2))
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok1=false,ok2=false;
		sIndex=m.args[firstIndexArgument].toULongLong(&ok1);
		eIndex=m.args[firstIndexArgument+1].toULongLong(&ok2);
		if(!ok1||!ok2)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		for(quint64 i=sIndex;i<=eIndex;++i)
		{
			ARpcISensorValue *val=st->valueAt(i);
			if(!val)continue;
			clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,val->dump());
		}
		return true;
	}
	return false;
}

QStringList GetSamplesCommand::acceptedCommands()
{
	return QStringList()<<"get_samples_count"<<"get_samples";
}
