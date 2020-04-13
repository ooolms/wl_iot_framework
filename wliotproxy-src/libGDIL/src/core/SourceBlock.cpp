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

#include "GDIL/core/SourceBlock.h"

SourceBlock::SourceBlock(quint32 id)
	:BaseBlock(id)
	,mNextData(DataUnit::SINGLE,1)
	,mWorkData(DataUnit::SINGLE,1)
{
}

SourceBlock::~SourceBlock()
{
}

bool SourceBlock::extractData()
{
	if(outputsCount()==0)
		return false;
	mNextData=extractDataInternal();
	return mNextData.isValid();
}

bool SourceBlock::prepareWorkData()
{
	if(outputsCount()==0)
		return false;
	mWorkData=mNextData.mkCopy();
	mNextData=DataUnit(mWorkData.type(),mWorkData.dim());
	return mWorkData.isValid();
}

bool SourceBlock::isSourceBlock()const
{
	return true;
}

void SourceBlock::eval()
{
	if(outputsCount()==0)return;
	output(0)->setData(mWorkData);
}
