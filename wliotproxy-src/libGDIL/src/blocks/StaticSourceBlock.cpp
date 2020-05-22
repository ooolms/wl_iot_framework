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

using namespace WLIOT;
using namespace WLIOTGDIL;

const QString StaticSourceBlock::mBlockName=QString("static_source");

StaticSourceBlock::StaticSourceBlock(quint32 bId)
	:SourceBlock(bId)
	,mValue(0.0)
{
	mMakeConfigOption=false;
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

bool StaticSourceBlock::configurable()const
{
	return mMakeConfigOption;
}

void StaticSourceBlock::setParams(const DataUnit &u,bool configurable)
{
	if(!u.isValid())return;
	mValue=u;
	output(0)->replaceTypeAndDim(mValue.type(),mValue.dim());

	if(configurable!=mMakeConfigOption)
	{
		mMakeConfigOption=configurable;
		if(!mMakeConfigOption)
			rmConfigOption("value");
		else addConfigOption("value",TypeConstraints(DataUnit::ANY,0),mValue);
	}
}

DataUnit StaticSourceBlock::extractDataInternal()
{
	return mValue;
}

void StaticSourceBlock::onConfigOptionChanged(const QByteArray &key)
{
	if(key=="value")
		setParams(configOptionValue(key),mMakeConfigOption);
}
