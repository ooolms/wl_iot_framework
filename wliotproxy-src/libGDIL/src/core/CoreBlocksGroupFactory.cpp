#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/blocks/AllBlocks.h"

const QString CoreBlocksGroupFactory::mGroupName=QString("core");

QString CoreBlocksGroupFactory::groupName()const
{
	return mGroupName;
}

QStringList CoreBlocksGroupFactory::allBlocks()const
{
	return QStringList();
}

BaseBlock* CoreBlocksGroupFactory::makeBlock(const QString &name,quint32 blockId)
{
	if(name==AverageCalcBlock::mBlockName)
		return new AverageCalcBlock(blockId);
	else if(name==CommandBlock::mBlockName)
		return new CommandBlock(blockId);
	else if(name==ComparationBlock::mBlockName)
		return new ComparationBlock(blockId);
	else if(name==DimChangeBlock::mBlockName)
		return new DimChangeBlock(blockId);
	else if(name==ConditionTransactionBlock::mBlockName)
		return new ConditionTransactionBlock(blockId);
	else if(name==NormingBlock::mBlockName)
		return new NormingBlock(blockId);
	else if(name==StaticSourceBlock::mBlockName)
		return new StaticSourceBlock(blockId);
	else if(name==StorageSourceBlock::mBlockName)
		return new StorageSourceBlock(blockId);
	else if(name==DebugBlock::mBlockName)
		return new DebugBlock(blockId);
	else return 0;
}
