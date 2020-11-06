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

#include "VDIL/blocks/CommandBlock.h"
#include <VDIL/core/Program.h>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString CommandBlock::mBlockName=QString("command");

CommandBlock::CommandBlock(quint32 bId)
	:BaseBlock(bId)
{
	mArgInCount=1;
	condInput=0;
	mEnableConditionInput=false;
	mOutMode=NoOut;
	out=0;
	argsInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::BOOL,1),
		DataUnit::SINGLE,"in "+QString::number(1)));
}

void CommandBlock::setParams(const QUuid &devId,const QByteArray &cmd,
	const QByteArrayList &args,quint32 inCount,bool enableConditionInput,OutputMode outMode)
{
	mDevId=devId;
	mCmd=cmd;
	mArgs=args;
	if(inCount<mArgInCount)
		for(quint32 i=inCount;i<mArgInCount;++i)
		{
			rmInput(argsInputs.last());
			argsInputs.removeLast();
		}
	else if(inCount>mArgInCount)
		for(quint32 i=mArgInCount;i<inCount;++i)
			argsInputs.append(mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::BOOL,1),
				DataUnit::SINGLE,"in "+QString::number(i+1)));
	mArgInCount=inCount;
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
	if(mOutMode!=outMode)
	{
		if(mOutMode==NoOut&&outMode!=NoOut)
			out=mkOutput(TypeAndDim(DataUnit::BOOL,1),"out");
		else if(mOutMode!=NoOut&&outMode==NoOut)
		{
			rmOutput(out);
			out=0;
		}
		mOutMode=outMode;
	}
}

QString CommandBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString CommandBlock::blockName()const
{
	return mBlockName;
}

const QUuid &CommandBlock::deviceId()const
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

quint32 CommandBlock::argInputsCount()const
{
	return mArgInCount;
}

bool CommandBlock::enableConditionInput()const
{
	return mEnableConditionInput;
}

CommandBlock::OutputMode CommandBlock::outMode()const
{
	return mOutMode;
}

QString CommandBlock::outModeToStr(CommandBlock::OutputMode m)
{
	if(m==NotifyOnly)
		return "notify";
	else if(m==WaitForCommandAnswer)
		return "wait";
	else return "no";
}

CommandBlock::OutputMode CommandBlock::outModeFromStr(const QString &s)
{
	if(s=="notify")
		return NotifyOnly;
	else if(s=="wait")
		return WaitForCommandAnswer;
	else return NoOut;
}

void CommandBlock::eval()
{
	if(!engineCallbacks()||mCmd.isEmpty()||mDevId.isNull())return;
	if(mEnableConditionInput)
	{
		if(!condInput->data().boolValue())
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
	if(mOutMode==WaitForCommandAnswer)
	{
		QByteArrayList retVal;
		bool ok=engineCallbacks()->commandCallbackWaitAnswer(mDevId,mCmd,args,retVal);
		out->setData(DataUnit(ok));
	}
	else
	{
		engineCallbacks()->commandCallback(mDevId,mCmd,args);
		if(mOutMode==NotifyOnly)
			out->setData(DataUnit(true));
	}
}

QList<QUuid> CommandBlock::usedDevices()const
{
	QList<QUuid> l;
	if(!mDevId.isNull())
		l.append(mDevId);
	return l;
}
