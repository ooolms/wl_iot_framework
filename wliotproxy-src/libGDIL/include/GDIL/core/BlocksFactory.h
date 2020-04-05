#ifndef BLOCKSFACTORY_H
#define BLOCKSFACTORY_H

#include "GDIL/core/BaseBlock.h"

class Program;

class BlocksFactory
{
public:
	BaseBlock* makeBlock(const QUuid &type,quint32 blocKId=0);
};

#endif // EMBEDDEDBLOCKSFACTORY_H
