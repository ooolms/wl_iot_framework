#ifndef NORMINGBLOCK_H
#define NORMINGBLOCK_H

#include "GIDL/core/BaseBlock.h"

class NormingBlock
	:public BaseBlock
{
public:
	explicit NormingBlock(quint32 bId,const DataUnit &minX,const DataUnit &maxX,
		const DataUnit &minY,const DataUnit &maxY);

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

private:
	DataUnit mMinX,mMaxX,mMinY,mMaxY;
	BlockInput *in;
	BlockOutput *out;
};

#endif // NORMINGBLOCK_H
