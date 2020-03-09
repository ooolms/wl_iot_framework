#ifndef BLOCKINPUT_H
#define BLOCKINPUT_H

#include "GIDL/core/DataUnit.h"

class BlockOutput;
class BaseBlock;

class BlockInput
{
public:
	explicit BlockInput(BaseBlock *b,DataUnit::Types suppTypes,DataUnit::Type currType);
	~BlockInput();

	//for program editing
	DataUnit::Types supportedTypes()const;
	DataUnit::Type currentType()const;
	bool canConnectType(DataUnit::Type t)const;

	//for work
	void reset();
	bool setData(DataUnit u);
	bool isReady();

private:
	//for program editing
	DataUnit::Types mSupportedTypes;
	DataUnit::Type mCurrentType;
	BlockOutput *mLinkedOutput;
	BaseBlock *mBlock;

	//for work
	bool mDataIsSet;
	DataUnit mData;

	friend class BlockOutput;
};

#endif // BLOCKINPUT_H
