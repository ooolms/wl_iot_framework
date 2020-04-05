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
	else if(type==EqTransactionBlock::mTypeId)
		return new EqTransactionBlock(blocKId);
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
	else if(type==NormingBlock::mTypeId)
		return new NormingBlock(blocKId);
	else if(type==StaticSourceBlock::mTypeId)
		return new StaticSourceBlock(blocKId);
	else if(type==StorageSourceBlock::mTypeId)
		return new StorageSourceBlock(blocKId);
	else return 0;
}
