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

#ifndef COREBLOCKSGROUPFACTORY_H
#define COREBLOCKSGROUPFACTORY_H

#include "GDIL/core/IBlocksGroupFactory.h"

namespace WLIOTGDIL
{
	class CoreBlocksGroupFactory
		:public IBlocksGroupFactory
	{
	public:
		virtual QString groupName()const override;
		virtual QStringList allBlocks()const override;
		virtual BaseBlock *makeBlock(const QString &name,quint32 blockId)override;

	public:
		static const QString mGroupName;
	};
}

#endif // COREBLOCKSGROUPFACTORY_H
