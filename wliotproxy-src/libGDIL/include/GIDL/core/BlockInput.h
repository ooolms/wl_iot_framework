#ifndef BLOCKINPUT_H
#define BLOCKINPUT_H

#include "GIDL/core/DataUnit.h"

class BlockOutput;
class BaseBlock;

//supported dim: 0 - any dim, >=1 - only specified dim
class BlockInput
{
public:
	explicit BlockInput(BaseBlock *b,DataUnit::Types suppTypes,DataUnit::Type currType,quint32 supportedDim);
	~BlockInput();

	//for program editing
	DataUnit::Types supportedTypes()const;
	DataUnit::Type currentType()const;
	quint32 supportedDim()const;
	quint32 currentDim()const;
	bool canConnectType(DataUnit::Type t,quint32 dim)const;

	//for work
	void reset();
	bool setData(DataUnit u);
	bool isReady();
	const DataUnit& data();

private:
	//for program editing
	DataUnit::Types mSupportedTypes;
	quint32 mSupportedDim;
	DataUnit::Type mCurrentType;
	quint32 mCurrentDim;
	BlockOutput *mLinkedOutput;
	BaseBlock *mBlock;

	//for work
	bool mDataIsSet;
	DataUnit mData;

	friend class BlockOutput;
};

#endif // BLOCKINPUT_H
