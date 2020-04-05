#ifndef COMPARATIONBLOCK_H
#define COMPARATIONBLOCK_H

#include "GDIL/core/BaseBlock.h"

class ComparationBlock
	:public BaseBlock
{
public:
	enum Operation
	{
		EQ,
		NEQ,
		GT,
		LT,
		GTEQ,
		LTEQ,
		DISTANCE
	};

public:
	explicit ComparationBlock(quint32 bId=0);
	void setParams(bool boolOutMode,quint32 dimIndex,Operation operation);
	void setDistValue(double val);
	void setDistValue(qint64 val);
	void setDistValue(const DataUnit &val);
	virtual QUuid typeId()const override;
	bool boolOutMode()const;
	const DataUnit& distValue()const;
	quint32 dimIndex()const;
	Operation operation()const;

protected:
	virtual void eval()override;

private:
	void rmOutputs();
	void mkOutputs();
	void updateHint();

public:
	static const QUuid mTypeId;

private:
	BlockInput *in1,*in2;
	BlockOutput *out1,*out2;
	bool mBoolOutMode;
	DataUnit mDistValue;
	quint32 mDimIndex;
	Operation mOp;
};

#endif // COMPARATIONBLOCK_H
