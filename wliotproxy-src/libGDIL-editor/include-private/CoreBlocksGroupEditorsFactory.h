#ifndef COREBLOCKSGROUPEDITORSFACTORY_H
#define COREBLOCKSGROUPEDITORSFACTORY_H

#include "BaseBlocksGroupEditorsFactory.h"

class CoreBlocksGroupEditorsFactory
	:public BaseBlocksGroupEditorsFactory
{
public:
	CoreBlocksGroupEditorsFactory();
	virtual QString groupDisplayTitle()const override;
};

#endif // COREBLOCKSGROUPEDITORSFACTORY_H
