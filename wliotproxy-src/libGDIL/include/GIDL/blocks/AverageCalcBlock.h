#ifndef AVERAGECALCBLOCK_H
#define AVERAGECALCBLOCK_H

#include "GIDL/core/BaseBlock.h"

class AverageCalcBlock
	:public BaseBlock
{
public:
	explicit AverageCalcBlock(quint32 bId);

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

private:
	BlockInput *in;
	BlockOutput *out;
};

#endif // AVERAGECALCBLOCK_H
