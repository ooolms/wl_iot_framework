#ifndef DIMCHANGEBLOCK_H
#define DIMCHANGEBLOCK_H

#include "GIDL/core/BaseBlock.h"

class DimChangeBlock
	:public BaseBlock
{
public:
	explicit DimChangeBlock(quint32 dim,quint32 bId);

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

private:
	BlockInput *in;
	BlockOutput *out;
	quint32 mDim;

};

#endif // DIMCHANGEBLOCK_H
