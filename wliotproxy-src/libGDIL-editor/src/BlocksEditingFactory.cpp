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

#include "GDIL/editor/BlocksEditingFactory.h"
#include "CoreBlocksGroupEditorsFactory.h"
#include "LogicalBlocksGroupEditorsFactory.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/core/LogicalBlocksGroupFactory.h"

BlocksEditingFactory::BlocksEditingFactory()
{
	mGroupNames.append(CoreBlocksGroupFactory::mGroupName);
	mGroupNames.append(LogicalBlocksGroupFactory::mGroupName);
	mGroups[CoreBlocksGroupFactory::mGroupName]=new CoreBlocksGroupEditorsFactory;
	mGroups[LogicalBlocksGroupFactory::mGroupName]=new LogicalBlocksGroupEditorsFactory;
}

BlocksEditingFactory::~BlocksEditingFactory()
{
	for(auto v:mGroups)
		delete v;
}

IBlocksGroupEditorsFactory *BlocksEditingFactory::groupFactory(const QString &groupName)
{
	return mGroups.value(groupName);
}

IBlockEditor* BlocksEditingFactory::editor(const QString &groupName,const QString &blockName)
{
	IBlocksGroupEditorsFactory *f=mGroups.value(groupName);
	if(!f)return 0;
	return f->editor(blockName);
}

QStringList BlocksEditingFactory::allGroups()
{
	return mGroupNames;
}
