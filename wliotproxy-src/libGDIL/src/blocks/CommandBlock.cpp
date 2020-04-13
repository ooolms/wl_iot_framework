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

const QUuid CommandBlock::mTypeId=QUuid("{00b3a487-d520-425c-a2e5-136737727cc8}");

CommandBlock::CommandBlock(quint32 bId)
	:BaseBlock(bId)
{
	mInCount=1;
	mkInput(DataUnit::SINGLE|DataUnit::BOOL,DataUnit::SINGLE,1,"in "+QString::number(1));
}

void CommandBlock::setParams(const QUuid &devId,const QByteArray &devName,const QByteArray &cmd,
	const QByteArrayList &args,quint32 inCount)
{
	mDevId=devId;
	mDevName=devName;
	mCmd=cmd;
	mArgs=args;
	if(inCount<mInCount)
		for(quint32 i=inCount;i<mInCount;++i)
			rmInput(inputsCount()-1);
	else if(inCount>mInCount)
		for(quint32 i=mInCount;i<inCount;++i)
			mkInput(DataUnit::SINGLE|DataUnit::BOOL,
				DataUnit::SINGLE,1,"in "+QString::number(i+1));
	mInCount=inCount;
	hint=QString::fromUtf8(mDevName)+"\ncmd: "+mCmd+"\nargs: "+mArgs.join("|");
}

QUuid CommandBlock::typeId()const
{
	return mTypeId;
}

const QUuid &CommandBlock::devId() const
{
	return mDevId;
}

const QByteArray &CommandBlock::devName() const
{
	return mDevName;
}

const QByteArray &CommandBlock::cmd() const
{
	return mCmd;
}

const QByteArrayList &CommandBlock::args() const
{
	return mArgs;
}

quint32 CommandBlock::inCount()const
{
	return mInCount;
}

void CommandBlock::eval()
{
	if(!prg->engineCallbacks()||mCmd.isEmpty()||mDevId.isNull())return;
	QByteArrayList args=mArgs;
	QByteArrayList inputStrs;
	for(quint32 i=0;i<inputsCount();++i)
		inputStrs.append(input(i)->data().value()->valueToString(0));
	for(int i=0;i<args.count();++i)
	{
		QByteArray &arg=args[i];
		for(int j=0;j<inputStrs.count();++j)
			arg.replace("${"+QByteArray::number(j)+"}",inputStrs[j]);
	}
	prg->engineCallbacks()->commandCallback(mDevId,mCmd,args);
}
