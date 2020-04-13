#ifndef IBLOCKSGROUPEDITORSFACTORY_H
#define IBLOCKSGROUPEDITORSFACTORY_H

#include "IBlockEditor.h"

class IBlocksGroupEditorsFactory
{
public:
	virtual ~IBlocksGroupEditorsFactory(){}
	virtual QString groupDisplayTitle()const=0;
	virtual QStringList allBlocks()const=0;
	virtual IBlockEditor* editor(const QString &blockName)=0;
};


#endif // IBLOCKSGROUPEDITORSFACTORY_H
