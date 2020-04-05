#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "GDIL/core/BlockInput.h"
#include "GDIL/core/BlockOutput.h"
#include "GDIL/core/IEngineHelper.h"
#include <QList>
#include <QPoint>

class Program;

class BaseBlock
{
public:
	explicit BaseBlock(quint32 bId);
	virtual ~BaseBlock();
	virtual QUuid typeId()const=0;

	//for program
	int inputsCount();
	BlockInput* input(int index);
	int inputIndex(const BlockInput *in)const;
	int outputsCount();
	BlockOutput* output(int index);
	int outputIndex(const BlockOutput *out)const;

	//for work
	void evalIfReady();

protected:
	virtual void eval()=0;
	virtual void onInputTypeSelected(BlockInput *b);

public:
	//editing attributes
	QString title;
	QString hint;
	QPointF position;
	quint32 mBlockId;

protected:
	Program *prg;
	QList<BlockInput*> inputs;
	QList<BlockOutput*> outputs;
	friend class BlockOutput;
	friend class Program;
};

#endif // BASEBLOCK_H
