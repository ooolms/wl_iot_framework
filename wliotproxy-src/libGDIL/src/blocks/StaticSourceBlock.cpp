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

#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

const QString StaticSourceBlock::mBlockName=QString("static_source");

StaticSourceBlock::StaticSourceBlock(quint32 bId)
	:SourceBlock(bId)
	,mValue(0.0)
{
	if(mValue.isValid())
		mkOutput(mValue.type(),mValue.dim(),"out");
}

QString StaticSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString StaticSourceBlock::blockName()const
{
	return mBlockName;
}

const DataUnit& StaticSourceBlock::value()const
{
	return mValue;
}

void StaticSourceBlock::setValue(const DataUnit &u)
{
	mValue=u;
	if(mValue.isValid())
	{
		if(outputsCount()>0)
			output(0)->replaceTypeAndDim(mValue.type(),mValue.dim());
		else mkOutput(mValue.type(),mValue.dim(),"out");
	}
	else if(outputsCount()>0)
		rmOutput(0);

	if(!mValue.isValid())
		hint.clear();
	else if(mValue.type()==DataUnit::BOOL)
		hint=QString("bool: ")+(mValue.value()->valueToS64(0)==1?"true":"false");
	else if(mValue.type()==DataUnit::ARRAY)
		hint=QString("array: ")+mValue.value()->dumpToMsgArgs().join("|");
	else hint=QString("single: ")+mValue.value()->valueToString(0);
}

DataUnit StaticSourceBlock::extractDataInternal()
{
	return mValue;
}
