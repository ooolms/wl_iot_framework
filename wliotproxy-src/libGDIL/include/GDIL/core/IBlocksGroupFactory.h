#ifndef IBLOCKSGROUPFACTORY_H
#define IBLOCKSGROUPFACTORY_H

#include "GDIL/core/BaseBlock.h"

class IBlocksGroupFactory
{
public:
	virtual ~IBlocksGroupFactory(){}
	virtual QString groupName()const=0;
	virtual QStringList allBlocks()const=0;
	virtual BaseBlock *makeBlock(const QString &name,quint32 blockId)=0;
};

#endif // IBLOCKSGROUPFACTORY_H
