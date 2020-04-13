#ifndef LOGICALBLOCKSGROUPEDITORSFACTORY_H
#define LOGICALBLOCKSGROUPEDITORSFACTORY_H

#include "BaseBlocksGroupEditorsFactory.h"

class LogicalBlocksGroupEditorsFactory
	:public BaseBlocksGroupEditorsFactory
{
public:
	LogicalBlocksGroupEditorsFactory();
	virtual QString groupDisplayTitle()const override;
};

#endif // LOGICALBLOCKSGROUPEDITORSFACTORY_H
