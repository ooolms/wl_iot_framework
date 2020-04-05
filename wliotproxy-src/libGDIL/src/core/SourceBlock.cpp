#include "GDIL/core/SourceBlock.h"

SourceBlock::SourceBlock(quint32 id)
	:BaseBlock(id)
	,mNextData(DataUnit::SINGLE,1)
	,mWorkData(DataUnit::SINGLE,1)
{
	out=0;
}

SourceBlock::~SourceBlock()
{
}

bool SourceBlock::extractData()
{
	mNextData=extractDataInternal();
	return mNextData.isValid();
}

bool SourceBlock::prepareWorkData()
{
	mWorkData=mNextData.mkCopy();
	mNextData=DataUnit(mWorkData.type(),mWorkData.dim());
	return mWorkData.isValid();
}

void SourceBlock::eval()
{
	out->setData(mWorkData);
}

void SourceBlock::setOutput(BlockOutput *o)
{
	if(out)
		delete out;
	outputs.clear();
	out=o;
	if(out)
		outputs.append(out);
}
