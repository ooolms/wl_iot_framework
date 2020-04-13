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
#include "GDIL/blocks/AllBlocks.h"

BaseBlock* BlocksFactory::makeBlock(const QUuid &type,quint32 blocKId)
{
	if(type==AverageCalcBlock::mTypeId)
		return new AverageCalcBlock(blocKId);
	else if(type==CommandBlock::mTypeId)
		return new CommandBlock(blocKId);
	else if(type==ComparationBlock::mTypeId)
		return new ComparationBlock(blocKId);
	else if(type==DimChangeBlock::mTypeId)
		return new DimChangeBlock(blocKId);
	else if(type==ConditionTransactionBlock::mTypeId)
		return new ConditionTransactionBlock(blocKId);
	else if(type==NotBoolBlock::mTypeId)
		return new NotBoolBlock(blocKId);
	else if(type==AndBoolBlock::mTypeId)
		return new AndBoolBlock(blocKId);
	else if(type==OrBoolBlock::mTypeId)
		return new OrBoolBlock(blocKId);
	else if(type==XorBoolBlock::mTypeId)
		return new XorBoolBlock(blocKId);
	else if(type==AndNotBoolBlock::mTypeId)
		return new AndNotBoolBlock(blocKId);
	else if(type==OrNotBoolBlock::mTypeId)
		return new OrNotBoolBlock(blocKId);
	else if(type==XorNotBoolBlock::mTypeId)
		return new XorNotBoolBlock(blocKId);
	else if(type==NormingBlock::mTypeId)
		return new NormingBlock(blocKId);
	else if(type==StaticSourceBlock::mTypeId)
		return new StaticSourceBlock(blocKId);
	else if(type==StorageSourceBlock::mTypeId)
		return new StorageSourceBlock(blocKId);
	else if(type==DebugBlock::mTypeId)
		return new DebugBlock(blocKId);
	else return 0;
}
