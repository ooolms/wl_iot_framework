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

#include "GDIL/blocks/CommandBlock.h"
#include "GDIL/core/Program.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

const QString CommandBlock::mBlockName=QString("command");

CommandBlock::CommandBlock(quint32 bId)
	:BaseBlock(bId)
{
	mInCount=1;
	condInput=0;
	mEnableConditionInput=false;
	argsInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::BOOL,1),
		DataUnit::SINGLE,"in "+QString::number(1)));
}

void CommandBlock::setParams(const QUuid &devId,const QByteArray &cmd,
	const QByteArrayList &args,quint32 inCount,bool enableConditionInput)
{
	mDevId=devId;
	mCmd=cmd;
	mArgs=args;
	if(inCount<mInCount)
		for(quint32 i=inCount;i<mInCount;++i)
		{
			rmInput(argsInputs.last());
			argsInputs.removeLast();
		}
	else if(inCount>mInCount)
		for(quint32 i=mInCount;i<inCount;++i)
			argsInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::BOOL,1),
				DataUnit::SINGLE,"in "+QString::number(i+1)));
	mInCount=inCount;
	if(mEnableConditionInput!=enableConditionInput)
	{
		mEnableConditionInput=enableConditionInput;
		if(mEnableConditionInput)
			condInput=mkInput(TypeConstraints(DataUnit::BOOL,1),DataUnit::BOOL,"condition",0);
		else
		{
			rmInput(condInput);
			condInput=0;
		}
	}
	updateDevNames();
	hint="device: ";
	if(prg)
	{
		hint+=prg->findDevName(mDevId);
		hint+=" ("+mDevId.toString()+")";
	}
	else hint+=mDevId.toString();
	hint+="\ncmd: "+QString::fromUtf8(mCmd)+"\nargs: "+QString::fromUtf8(mArgs.join("|"));
}

QString CommandBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString CommandBlock::blockName()const
{
	return mBlockName;
}

const QUuid &CommandBlock::devId()const
{
	return mDevId;
}

const QByteArray& CommandBlock::cmd()const
{
	return mCmd;
}

const QByteArrayList &CommandBlock::args()const
{
	return mArgs;
}

quint32 CommandBlock::inCount()const
{
	return mInCount;
}

bool CommandBlock::enableConditionInput()const
{
	return mEnableConditionInput;
}

void CommandBlock::eval()
{
	if(!engineCallbacks()||mCmd.isEmpty()||mDevId.isNull())return;
	if(mEnableConditionInput)
	{
		if(condInput->data().value()->valueToS64(0)!=1)
			return;
	}
	QByteArrayList args=mArgs;
	QByteArrayList inputStrs;
	for(quint32 i=0;i<(quint32)inputsCount();++i)
		inputStrs.append(input(i)->data().value()->valueToString(0));
	for(int i=0;i<args.count();++i)
	{
		QByteArray &arg=args[i];
		for(int j=0;j<inputStrs.count();++j)
			arg.replace("${"+QByteArray::number(j)+"}",inputStrs[j]);
	}
	engineCallbacks()->commandCallback(mDevId,mCmd,args);
}

QList<QUuid> CommandBlock::usedDevices()const
{
	QList<QUuid> l;
	if(!mDevId.isNull())
		l.append(mDevId);
	return l;
}