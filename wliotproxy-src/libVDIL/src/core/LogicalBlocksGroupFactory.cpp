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

#include "VDIL/core/LogicalBlocksGroupFactory.h"
#include "VDIL/blocks/LogicalBlocks.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString LogicalBlocksGroupFactory::mGroupName=QString("logic");

QString LogicalBlocksGroupFactory::groupName()const
{
	return mGroupName;
}

QStringList LogicalBlocksGroupFactory::allBlocks()const
{
	return QStringList();
}

BaseBlock* LogicalBlocksGroupFactory::makeBlock(const QString &name,quint32 blockId)
{
	if(name==NotBoolBlock::mBlockName)
		return new NotBoolBlock(blockId);
	else if(name==AndBoolBlock::mBlockName)
		return new AndBoolBlock(blockId);
	else if(name==OrBoolBlock::mBlockName)
		return new OrBoolBlock(blockId);
	else if(name==XorBoolBlock::mBlockName)
		return new XorBoolBlock(blockId);
	else if(name==AndNotBoolBlock::mBlockName)
		return new AndNotBoolBlock(blockId);
	else if(name==OrNotBoolBlock::mBlockName)
		return new OrNotBoolBlock(blockId);
	else if(name==XorNotBoolBlock::mBlockName)
		return new XorNotBoolBlock(blockId);
	else return 0;
}
