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

bool GetSamplesCommand::processCommand(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId;
	QByteArray sensorName(m.args[1]);
	ARpcISensorStorage *st=IotProxyInstance::inst().sensorsStorage()->findStorageForDevice(
		m.args[0],sensorName,deviceId);
	if(!st||deviceId.isNull())
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
			retVal.append("can't open session: "+sessionId.toByteArray());
			return false;
		}
	}
	if(m.title=="get_samples_count")
	{
		if(!st->isOpened()&&!st->open())
		{
			retVal.append("can't open storage");
			return false;
		}
		retVal.append(QByteArray::number(st->valuesCount()));
		return true;
	}
	else if(m.title=="get_samples")
	{
		quint64 sIndex,count;
		if(m.args.count()<(firstIndexArgument+2))
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok1=false,ok2=false,ok3=true;
		sIndex=m.args[firstIndexArgument].toULongLong(&ok1);
		count=m.args[firstIndexArgument+1].toULongLong(&ok2);
		quint64 step=1;
		if(m.args.count()==(firstIndexArgument+3))
			step=m.args[firstIndexArgument+2].toULongLong(&ok3);
		if(!ok1||!ok2||!ok3)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(step==0)step=1;
		if(!st->isOpened()&&!st->open())
		{
			retVal.append("can't open storage");
			return false;
		}
		if((sIndex+step*(count-1))>=st->valuesCount())
		{
			retVal.append("indexes out of range");
			return false;
		}
		for(quint64 i=0;i<count;++i)
		{
			ARpcISensorValue *val=st->valueAt(sIndex+i*step);
			if(!val)continue;
			clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,val->dump());
		}
		return true;
	}
	return false;
}

QByteArrayList GetSamplesCommand::acceptedCommands()
{
	return QByteArrayList()<<"get_samples_count"<<"get_samples";
}
