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

#include "VDIL/core/BlocksFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

BlocksFactory::BlocksFactory()
{
}

BlocksFactory::~BlocksFactory()
{
	for(IBlocksGroupFactory *g:mGroups)
		delete g;
	mGroups.clear();
}

QStringList BlocksFactory::allGroups()
{
	return mGroups.keys();
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

bool BlocksFactory::registerGroupFactory(IBlocksGroupFactory *f)
{
	if(mGroups.contains(f->groupName()))
		return false;
	mGroups[f->groupName()]=f;
	return true;
}
