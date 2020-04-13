#include "GDIL/core/LogicalBlocksGroupFactory.h"
#include "GDIL/blocks/AllBlocks.h"

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
