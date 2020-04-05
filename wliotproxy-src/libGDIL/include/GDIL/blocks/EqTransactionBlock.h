#ifndef EQTRANSACTIONBLOCK_H
#define EQTRANSACTIONBLOCK_H

#include "GDIL/core/BaseBlock.h"

class EqTransactionBlock
	:public BaseBlock
{
public:
	explicit EqTransactionBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QUuid mTypeId;

private:
	BlockInput *boolIn;
	BlockInput *in;
	BlockOutput *out;
};

#endif // EQTRANSACTIONBLOCK_H
