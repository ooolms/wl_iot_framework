#ifndef AVERAGECALCBLOCK_H
#define AVERAGECALCBLOCK_H

#include "GDIL/core/BaseBlock.h"

class AverageCalcBlock
	:public BaseBlock
{
public:
	explicit AverageCalcBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QUuid mTypeId;

private:
	BlockInput *in;
	BlockOutput *out;
};

#endif // AVERAGECALCBLOCK_H
