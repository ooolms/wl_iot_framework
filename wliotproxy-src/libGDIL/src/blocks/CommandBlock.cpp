#include "GDIL/blocks/CommandBlock.h"
#include "GDIL/core/Program.h"

const QUuid CommandBlock::mTypeId=QUuid("{00b3a487-d520-425c-a2e5-136737727cc8}");

CommandBlock::CommandBlock(quint32 bId)
	:BaseBlock(bId)
{
	mInputsCount=1;
	inputs.append(new BlockInput(this,DataUnit::SINGLE|DataUnit::BOOL,DataUnit::SINGLE,1,"in "+QString::number(1)));
}

void CommandBlock::setParams(const QUuid &devId,const QByteArray &devName,const QByteArray &cmd,
	const QByteArrayList &args,quint32 inCount)
{
	mDevId=devId;
	mDevName=devName;
	mCmd=cmd;
	mArgs=args;
	if(inCount<mInputsCount)
		for(quint32 i=inCount;i<mInputsCount;++i)
			delete inputs.takeLast();
	else if(inCount>mInputsCount)
		for(quint32 i=mInputsCount;i<inCount;++i)
			inputs.append(new BlockInput(this,DataUnit::SINGLE|DataUnit::BOOL,
				DataUnit::SINGLE,1,"in "+QString::number(i+1)));
	mInputsCount=inCount;
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

quint32 CommandBlock::inputsCount() const
{
	return mInputsCount;
}

void CommandBlock::eval()
{
	if(!prg->commandCallback()||mCmd.isEmpty()||mDevId.isNull())return;
	QByteArrayList args=mArgs;
	QByteArrayList inputStrs;
	for(int i=0;i<inputs.count();++i)
		inputs[i]->data().value()->valueToString(0);
	for(int i=0;i<args.count();++i)
	{
		QByteArray &arg=args[i];
		for(int j=0;j<inputStrs.count();++i)
			arg.replace("${"+QByteArray::number(j)+"}",inputStrs[j]);
	}
	prg->commandCallback()->commandCallback(mDevId,mCmd,args);
}
