#ifndef NORMINGBLOCK_H
#define NORMINGBLOCK_H

#include "GDIL/core/BaseBlock.h"

class NormingBlock
	:public BaseBlock
{
public:
	explicit NormingBlock(quint32 bId=0);
	void setParams(double minX,double maxX,double minY,double maxY,quint32 dimIndex,bool forceLimits);
	void setParams(qint64 minX,qint64 maxX,qint64 minY,qint64 maxY,quint32 dimIndex,bool forceLimits);
	const DataUnit& minX()const;
	const DataUnit& maxX()const;
	const DataUnit& minY()const;
	const DataUnit& maxY()const;
	quint32 dimIndex()const;
	bool forceLimits()const;
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

private:
	void updateHint();

public:
	static const QUuid mTypeId;

private:
	DataUnit mMinX,mMaxX,mMinY,mMaxY;
	quint32 mDimIndex;
	bool mForceLimits;
	BlockInput *in;
	BlockOutput *out;
};

#endif // NORMINGBLOCK_H
