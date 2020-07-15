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

#include "VDIL/blocks/DebugBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString DebugBlock::mBlockName=QString("debug");

DebugBlock::DebugBlock(quint32 bId)
	:BaseBlock(bId)
{
	mArgInputsCount=1;
	mkInput(TypeConstraints(DataUnit::ANY,0),DataUnit::SINGLE,"in "+QString::number(1));
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

quint32 DebugBlock::argInputsCount()const
{
	return mArgInputsCount;
}

void DebugBlock::setParams(const QString &debugString,quint32 argInputsCount)
{
	mDebugString=debugString;
	if(argInputsCount<mArgInputsCount)
		for(quint32 i=argInputsCount;i<mArgInputsCount;++i)
			rmInput(inputsCount()-1);
	else if(argInputsCount>mArgInputsCount)
		for(quint32 i=mArgInputsCount;i<argInputsCount;++i)
			mkInput(TypeConstraints(DataUnit::ANY,0),DataUnit::SINGLE,"in "+QString::number(i+1));
	mArgInputsCount=argInputsCount;
}

void DebugBlock::eval()
{
	if(!engineCallbacks())return;
	QStringList inputStrs;
	for(int i=0;i<inputsCount();++i)
	{
		BlockInput *in=input(i);
		if(in->type()==DataUnit::DATETIME)
			inputStrs.append(input(i)->data().dateTimeValue().toString());
		else inputStrs.append(QString::fromUtf8(input(i)->data().value()->dumpToMsgArgs().join('|')));
	}
	QString s=mDebugString;
	for(int i=0;i<inputStrs.count();++i)
		s.replace(QString::fromUtf8("${"+QByteArray::number(i)+"}"),inputStrs[i]);
	if(engineCallbacks())
		engineCallbacks()->debugCallback(s);
}
