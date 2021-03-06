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

#include "VDIL/blocks/StaticSourceBlock.h"
#include <VDIL/core/Program.h>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString StaticSourceBlock::mBlockName=QString("static_source");

StaticSourceBlock::StaticSourceBlock(quint32 bId)
	:SourceBlock(bId)
	,mValue(0.0)
{
	mConfigurable=false;
	out=mkOutput(mValue.typeAndDim(),"out");
}

QString StaticSourceBlock::groupName()const
{
	return Program::reservedCoreGroupName;
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
	return mConfigurable;
}

void StaticSourceBlock::setParams(const DataUnit &u,bool configurable)
{
	if(!u.isValid())return;
	DataUnit::Type prevType=mValue.type();
	mValue=u;
	output(0)->replaceTypeAndDim(mValue.typeAndDim());
	if(mConfigurable==configurable)
	{
		if(configurable&&prevType!=mValue.type())
		{
			rmConfigOption("value");
			addConfigOption("value",mValue);
		}
	}
	else if(configurable)
		addConfigOption("value",mValue);
	else rmConfigOption("value");
	mConfigurable=configurable;
}

DataUnit StaticSourceBlock::extractDataInternal()
{
	return mValue;
}

void StaticSourceBlock::onConfigOptionChanged(const QByteArray &key)
{
	if(key=="value")
	{
		DataUnit u=configOptionValue(key);
		if(mValue.type()==u.type())
			mValue=u;
		else setConfigOption("value",mValue);
	}
}
