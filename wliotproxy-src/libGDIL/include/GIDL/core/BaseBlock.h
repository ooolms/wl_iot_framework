#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "GIDL/core/BlockInput.h"
#include "GIDL/core/BlockOutput.h"
#include <QList>

class BaseBlock
{
public:
	explicit BaseBlock(quint32 id);
	virtual ~BaseBlock();

	//for program
	int inputsCount();
	BlockInput* input(int index);
	int outputsCount();
	BlockOutput* ountput(int index);
	quint32 id();

	//for work
	void evalIfReady();

protected:
	virtual void eval()=0;
	virtual void onInputTypeSelected(BlockInput *b);

protected:
	QList<BlockInput*> inputs;
	QList<BlockOutput*> outputs;
	quint32 mBlockId;
	friend class BlockOutput;
};

#endif // BASEBLOCK_H
