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
#include "ARpcStorages/ARpcAllStorages.h"

GetSamplesCommand::GetSamplesCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool GetSamplesCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId;
	QByteArray sensorName(ctx.args[1]);
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		ctx.args[0],sensorName,deviceId);
	if(!st||deviceId.isNull())
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	QUuid sessionId;
	int firstIndexArgument=2;
	if(st->storeMode()==ARpcISensorStorage::MANUAL_SESSIONS||st->storeMode()==ARpcISensorStorage::AUTO_SESSIONS)
	{
		firstIndexArgument=3;
		if(ctx.args.count()<3)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		sessionId=QUuid(ctx.args[2]);
		ARpcSessionStorage *sStor=reinterpret_cast<ARpcSessionStorage*>(st);
		if(sessionId.isNull()||!sStor->setMainReadSessionId(sessionId))
		{
			ctx.retVal.append("can't open session: "+sessionId.toByteArray());
			return false;
		}
	}
	if(ctx.cmd=="get_samples_count")
	{
		if(!st->isOpened()&&!st->open())
		{
			ctx.retVal.append("can't open storage");
			return false;
		}
		ctx.retVal.append(QByteArray::number(st->valuesCount()));
		return true;
	}
	else if(ctx.cmd=="get_samples")
		return getSamples(ctx,firstIndexArgument,st);
	else if(ctx.cmd=="get_samples_bin")
		return getSamplesBin(ctx,firstIndexArgument,st);
	return false;
}

QByteArrayList GetSamplesCommand::acceptedCommands()
{
	return QByteArrayList()<<"get_samples_count"<<"get_samples"<<"get_samples_bin";
}

bool GetSamplesCommand::getSamples(ICommand::CallContext &ctx,int firstIndexArgument,ARpcISensorStorage *st)
{
	quint64 sIndex,count;
	if(ctx.args.count()<(firstIndexArgument+2))
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	bool ok1=false,ok2=false,ok3=true;
	sIndex=ctx.args[firstIndexArgument].toULongLong(&ok1);
	count=ctx.args[firstIndexArgument+1].toULongLong(&ok2);
	quint64 step=1;
	if(ctx.args.count()==(firstIndexArgument+3))
		step=ctx.args[firstIndexArgument+2].toULongLong(&ok3);
	if(!ok1||!ok2||!ok3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	if(step==0)step=1;
	if(!st->isOpened()&&!st->open())
	{
		ctx.retVal.append("can't open storage");
		return false;
	}
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *val=st->valueAt(sIndex+i*step);
		if(!val)continue;
		writeCmdataMsg(ctx.callId,val->dumpToMsgArgs());
	}
	return true;
}

bool GetSamplesCommand::getSamplesBin(
	ICommand::CallContext &ctx,int firstIndexArgument,ARpcISensorStorage *st)
{
	quint64 sIndex,count;
	if(ctx.args.count()<(firstIndexArgument+2))
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	bool ok1=false,ok2=false,ok3=true;
	sIndex=ctx.args[firstIndexArgument].toULongLong(&ok1);
	count=ctx.args[firstIndexArgument+1].toULongLong(&ok2);
	quint64 step=1;
	if(ctx.args.count()==(firstIndexArgument+3))
		step=ctx.args[firstIndexArgument+2].toULongLong(&ok3);
	if(!ok1||!ok2||!ok3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	if(step==0)step=1;
	if(!st->isOpened()&&!st->open())
	{
		ctx.retVal.append("can't open storage");
		return false;
	}
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *val=st->valueAt(sIndex+i*step);
		if(!val)continue;
		writeCmdataMsg(ctx.callId,QByteArrayList()<<val->dumpToBinary());
	}
	return true;
}
