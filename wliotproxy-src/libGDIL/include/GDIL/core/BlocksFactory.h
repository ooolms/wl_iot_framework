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

#ifndef BLOCKSFACTORY_H
#define BLOCKSFACTORY_H

#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/IBlocksGroupFactory.h"

namespace WLIOTGDIL
{
	class Program;

	class BlocksFactory
	{
	public:
		BlocksFactory();
		~BlocksFactory();
		QStringList allGroups();
		IBlocksGroupFactory* groupFactory(const QString &groupName);
		BaseBlock* makeBlock(const QString &groupName,const QString &blockName,quint32 blockId);

	private:
		QMap<QString,IBlocksGroupFactory*> mGroups;
	};
}

#endif // EMBEDDEDBLOCKSFACTORY_H
