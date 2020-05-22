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

#include "GDIL/core/BlocksFactory.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/core/LogicalBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

BlocksFactory::BlocksFactory()
{
	mGroups[CoreBlocksGroupFactory::mGroupName]=new CoreBlocksGroupFactory;
	mGroups[LogicalBlocksGroupFactory::mGroupName]=new LogicalBlocksGroupFactory;
}

BlocksFactory::~BlocksFactory()
{
	for(IBlocksGroupFactory *g:mGroups)
		delete g;
}

QStringList BlocksFactory::allGroups()
{
	return QStringList()<<CoreBlocksGroupFactory::mGroupName<<LogicalBlocksGroupFactory::mGroupName;
}

IBlocksGroupFactory *BlocksFactory::groupFactory(const QString &groupName)
{
	return mGroups.value(groupName);
}

BaseBlock* BlocksFactory::makeBlock(const QString &groupName,const QString &blockName,quint32 blockId)
{
	IBlocksGroupFactory *f=mGroups.value(groupName);
	if(!f)return 0;
	return f->makeBlock(blockName,blockId);
}
