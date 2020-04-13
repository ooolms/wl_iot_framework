#ifndef LOGICALBLOCKSGROUPFACTORY_H
#define LOGICALBLOCKSGROUPFACTORY_H

#include "GDIL/core/IBlocksGroupFactory.h"

class LogicalBlocksGroupFactory
	:public IBlocksGroupFactory
{
public:
	virtual QString groupName() const override;
	virtual QStringList allBlocks() const override;
	virtual BaseBlock *makeBlock(const QString &name, quint32 blockId) override;

public:
	static const QString mGroupName;
};

#endif // LOGICALBLOCKSGROUPFACTORY_H
