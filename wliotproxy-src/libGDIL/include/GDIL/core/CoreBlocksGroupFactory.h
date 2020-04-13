#ifndef COREBLOCKSGROUPFACTORY_H
#define COREBLOCKSGROUPFACTORY_H

#include "GDIL/core/IBlocksGroupFactory.h"

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

#endif // COREBLOCKSGROUPFACTORY_H
