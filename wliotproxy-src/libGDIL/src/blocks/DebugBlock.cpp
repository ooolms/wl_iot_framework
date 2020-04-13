#include "GDIL/blocks/DebugBlock.h"
#include "GDIL/core/Program.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

const QString DebugBlock::mBlockName=QString("debug");

DebugBlock::DebugBlock(quint32 bId)
	:BaseBlock(bId)
{
	mInCount=1;
	mkInput(DataUnit::SINGLE|DataUnit::BOOL|DataUnit::ARRAY,DataUnit::SINGLE,1,"in "+QString::number(1));
}

QString DebugBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DebugBlock::blockName()const
{
	return mBlockName;
}

QString DebugBlock::debugString()const
{
	return mDebugString;
}

quint32 DebugBlock::inCount()const
{
	return mInCount;
}

void DebugBlock::setParams(const QString &debugString,quint32 inCount)
{
	mDebugString=debugString;
	if(inCount<mInCount)
		for(quint32 i=inCount;i<mInCount;++i)
			rmInput(inputsCount()-1);
	else if(inCount>mInCount)
		for(quint32 i=mInCount;i<inCount;++i)
			mkInput(DataUnit::SINGLE|DataUnit::BOOL|DataUnit::ARRAY,
				DataUnit::SINGLE,1,"in "+QString::number(i+1));
	mInCount=inCount;
	hint=mDebugString;
}

void DebugBlock::eval()
{
	QStringList inputStrs;
	for(int i=0;i<inputsCount();++i)
		inputStrs.append(QString::fromUtf8(input(i)->data().value()->dumpToMsgArgs().join('|')));
	QString s=mDebugString;
	for(int i=0;i<inputStrs.count();++i)
		s.replace("${"+QByteArray::number(i)+"}",inputStrs[i]);
	prg->engineCallbacks()->debugCallback(s);
}
