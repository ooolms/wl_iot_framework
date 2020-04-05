#include "GDIL/blocks/DimChangeBlock.h"

const QUuid DimChangeBlock::mTypeId=QUuid("{b538b4f7-1c68-4744-8925-9807445aca94}");

template<class T>
static void copy1Dim(const SensorValueNumeric<T> *from,SensorValueNumeric<T> *to,quint32 dim)
{
	QVector<T> vals;
	vals.resize((int)from->packetsCount());
	for(quint32 i=0;i<from->packetsCount();++i)
		vals[i]=from->getValue(dim,i);
	to->setData(vals);
}

DimChangeBlock::DimChangeBlock(quint32 bId)
	:BaseBlock(bId)
{
	mDim=0;
	in=new BlockInput(this,DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,1,"in");
	inputs.append(in);
	out=new BlockOutput(this,DataUnit::SINGLE,1,"out");
	outputs.append(out);
	hint="select dim "+QByteArray::number(mDim);
}

void DimChangeBlock::setDim(quint32 d)
{
	mDim=d;
	hint="select dim "+QByteArray::number(mDim);
}

quint32 DimChangeBlock::dim() const
{
	return mDim;
}

QUuid DimChangeBlock::typeId()const
{
	return mTypeId;
}

void DimChangeBlock::eval()
{
	if(in->data().dim()>=mDim)
		return;
	SensorDef::Type t=in->data().value()->type();
	t.dim=1;
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
	if(in->data().numType()==DataUnit::S64)
		copy1Dim((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data(),mDim);
	else if(in->data().numType()==DataUnit::F64)
		copy1Dim((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data(),mDim);
	else return;
	out->setData(DataUnit(v.data()));
}

void DimChangeBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	delete out;
	out=new BlockOutput(this,in->type(),in->dim(),"out");
	outputs[0]=out;
}
