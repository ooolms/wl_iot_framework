#include "GDIL/blocks/LogicalBlocks.h"

const QUuid NotBoolBlock::mTypeId=QUuid("{129deea3-b71b-4a5d-a2fd-d5f9ca98172c}");
const QUuid AndBoolBlock::mTypeId=QUuid("{b8998c44-21da-4f1f-866a-000b9fdcdea3}");
const QUuid OrBoolBlock::mTypeId=QUuid("{61a3844a-1f06-48db-9998-b343c24c8c83}");
const QUuid XorBoolBlock::mTypeId=QUuid("{53afcd4d-0d1f-4431-b2e7-9e7d031a6d15}");
const QUuid AndNotBoolBlock::mTypeId=QUuid("{b80fbe03-9ba4-4df2-b2fe-453801ee89f5}");
const QUuid OrNotBoolBlock::mTypeId=QUuid("{10543f30-1126-480f-9a4b-113fc0078440}");

NotBoolBlock::NotBoolBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=new BlockInput(this,DataUnit::BOOL,DataUnit::BOOL,1,"in");
	out=new BlockOutput(this,DataUnit::BOOL,1,"out");
	hint="NOT";
}

QUuid NotBoolBlock::typeId() const
{
	return mTypeId;
}

void NotBoolBlock::eval()
{
	const SensorValueU8 *vIn=(const SensorValueU8*)in->data().value();
	bool bv=vIn->getValue(0)==1;
	QScopedPointer<SensorValueU8> vOut((SensorValueU8*)vIn->mkCopy());
	vOut.data()->setValue((!bv)?1:0,0);
	out->setData(DataUnit(vOut.data()));
}

Base2InputsBoolBlock::Base2InputsBoolBlock(quint32 bId)
	:BaseBlock(bId)
{
	in1=new BlockInput(this,DataUnit::BOOL,DataUnit::BOOL,1,"in 1");
	in2=new BlockInput(this,DataUnit::BOOL,DataUnit::BOOL,1,"in 2");
	out=new BlockOutput(this,DataUnit::BOOL,1,"out");
}


void Base2InputsBoolBlock::eval()
{
	const SensorValueU8 *v1=(const SensorValueU8*)in1->data().value();
	const SensorValueU8 *v2=(const SensorValueU8*)in2->data().value();
	QScopedPointer<SensorValueU8> v((SensorValueU8*)v2->mkCopy());
	v.data()->setValue(calc(v1->getValue(0),v2->getValue(0)),0);
	out->setData(DataUnit(v.data()));
}

AndBoolBlock::AndBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
	hint="AND";
}

QUuid AndBoolBlock::typeId() const
{
	return mTypeId;
}

quint8 AndBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1&&v2)?1:0;
}

OrBoolBlock::OrBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
	hint="OR";
}

QUuid OrBoolBlock::typeId() const
{
	return mTypeId;
}

quint8 OrBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1||v2)?1:0;
}

XorBoolBlock::XorBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
	hint="XOR";
}

QUuid XorBoolBlock::typeId() const
{
	return mTypeId;
}

quint8 XorBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1^v2)?1:0;
}

AndNotBoolBlock::AndNotBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
	hint="AND-NOT";
}

QUuid AndNotBoolBlock::typeId() const
{
	return mTypeId;
}

quint8 AndNotBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (!(v1&&v2))?1:0;
}

OrNotBoolBlock::OrNotBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
	hint="OR-NOT";
}

QUuid OrNotBoolBlock::typeId() const
{
	return mTypeId;
}

quint8 OrNotBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (!(v1||v2))?1:0;
}
