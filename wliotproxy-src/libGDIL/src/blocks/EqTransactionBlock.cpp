#include "GDIL/blocks/EqTransactionBlock.h"

const QUuid EqTransactionBlock::mTypeId=QUuid("{2faf94c6-9b09-4edc-a97b-8680e59a5500}");

EqTransactionBlock::EqTransactionBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=new BlockInput(this,DataUnit::ANY,DataUnit::SINGLE,0,"in");
	boolIn=new BlockInput(this,DataUnit::BOOL,DataUnit::BOOL,1,"condition");
	inputs.append(in);
	inputs.append(boolIn);
	out=new BlockOutput(this,DataUnit::SINGLE,1,"in");
	outputs.append(out);
}

QUuid EqTransactionBlock::typeId()const
{
	return mTypeId;
}

void EqTransactionBlock::eval()
{
	const SensorValueU8 *bVal=(const SensorValueU8*)boolIn->data().value();
	if(bVal->getValue(0)==1)
		out->setData(in->data());
}

void EqTransactionBlock::onInputTypeSelected(BlockInput *b)
{
	if(b!=in)return;
	delete out;
	out=new BlockOutput(this,in->type(),in->dim(),"out");
	outputs[0]=out;
}
