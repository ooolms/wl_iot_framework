/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef NORMINGBLOCK_H
#define NORMINGBLOCK_H

#include "GDIL/core/BaseBlock.h"

class NormingBlock
	:public BaseBlock
{
public:
	explicit NormingBlock(quint32 bId=0);
	virtual QString groupName()const override;
	virtual QString blockName()const override;
	void setParams(double minX,double maxX,double minY,double maxY,quint32 dimIndex,bool forceLimits);
	void setParams(qint64 minX,qint64 maxX,qint64 minY,qint64 maxY,quint32 dimIndex,bool forceLimits);
	const DataUnit& minX()const;
	const DataUnit& maxX()const;
	const DataUnit& minY()const;
	const DataUnit& maxY()const;
	quint32 dimIndex()const;
	bool forceLimits()const;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QString mBlockName;

private:
	DataUnit mMinX,mMaxX,mMinY,mMaxY;
	quint32 mDimIndex;
	bool mForceLimits;
	BlockInput *in;
	BlockOutput *out;
};

#endif // NORMINGBLOCK_H
