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

#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString DeviceStateSourceBlock::mBlockName=QString("device_state_source");

DeviceStateSourceBlock::DeviceStateSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	mBoolOut=false;
	mCmdState=true;
	mkOutput(TypeAndDim(DataUnit::SINGLE,1),"out");
}

QString DeviceStateSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DeviceStateSourceBlock::blockName()const
{
	return mBlockName;
}

void DeviceStateSourceBlock::setParams(
	const QUuid &devId,const QByteArray &stateKey,bool cmdState,bool boolOut,quint32 cmdStateIndex)
{
	mDevId=devId;
	mStateKey=stateKey;
	mCmdState=cmdState;
	if(mBoolOut!=boolOut)
	{
		mBoolOut=boolOut;
		if(mBoolOut)
			output(0)->replaceTypeAndDim(TypeAndDim(DataUnit::BOOL,1));
		else output(0)->replaceTypeAndDim(TypeAndDim(DataUnit::SINGLE,1));
	}
	mCmdStateIndex=cmdStateIndex;
}

QUuid DeviceStateSourceBlock::deviceId()const
{
	return mDevId;
}

QByteArray DeviceStateSourceBlock::stateKey()const
{
	return mStateKey;
}

bool DeviceStateSourceBlock::commandState()const
{
	return mCmdState;
}

bool DeviceStateSourceBlock::boolOut()const
{
	return mBoolOut;
}

quint32 DeviceStateSourceBlock::commandStateIndex()const
{
	return mCmdStateIndex;
}

void DeviceStateSourceBlock::setValFromTriggerSlot(const QByteArray &v)
{
	valFromTriggerSlot=v;
}

DataUnit DeviceStateSourceBlock::extractDataInternal()
{
	if(helper())
		return DataUnit(TypeAndDim(DataUnit::INVALID,1));
	QByteArray val;
	if(!valFromTriggerSlot.isEmpty())
	{
		val=valFromTriggerSlot;
		valFromTriggerSlot.clear();
	}
	else
	{
		DeviceState s;
		if(!helper()->devStateById(mDevId,s))
			return DataUnit(TypeAndDim(DataUnit::INVALID,1));
		if(mCmdState)
			val=s.commandParams.value(mStateKey).value(mCmdStateIndex);
		else val=s.additionalAttributes.value(mStateKey);
		if(val.isEmpty())
			return DataUnit(TypeAndDim(DataUnit::INVALID,1));
	}
	if(mBoolOut)
	{
		if(val=="0")
			return DataUnit(false);
		else if(val=="1")
			return DataUnit(true);
		else return DataUnit(TypeAndDim(DataUnit::INVALID,1));
	}
	else
	{
		bool ok=false;
		qint64 valS64=val.toLongLong(&ok);
		if(ok)
			return DataUnit(valS64);
		double valF64=val.toDouble(&ok);
		if(ok)
			return DataUnit(valF64);
		return DataUnit(TypeAndDim(DataUnit::INVALID,1));
	}
}

QList<QUuid> DeviceStateSourceBlock::usedDevices()const
{
	QList<QUuid> l;
	if(!mDevId.isNull())
		l.append(mDevId);
	return l;
}
