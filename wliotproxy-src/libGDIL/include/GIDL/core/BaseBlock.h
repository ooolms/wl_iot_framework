#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "GIDL/core/BlockInput.h"
#include "GIDL/core/BlockOutput.h"
#include <QList>

class BaseBlock
{
public:
	virtual ~BaseBlock();
	int inputsCount();
	BlockInput* input(int index);
	int outputsCount();
	BlockOutput* ountput(int index);

protected:
	QList<BlockInput*> inputs;
	QList<BlockOutput*> outputs;
};

#endif // BASEBLOCK_H
