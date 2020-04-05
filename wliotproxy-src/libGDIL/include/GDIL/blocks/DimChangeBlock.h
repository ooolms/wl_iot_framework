#ifndef DIMCHANGEBLOCK_H
#define DIMCHANGEBLOCK_H

#include "GDIL/core/BaseBlock.h"

class DimChangeBlock
	:public BaseBlock
{
public:
	explicit DimChangeBlock(quint32 bId=0);
	void setDim(quint32 d);
	quint32 dim()const;
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QUuid mTypeId;

private:
	BlockInput *in;
	BlockOutput *out;
	quint32 mDim;
};

#endif // DIMCHANGEBLOCK_H
