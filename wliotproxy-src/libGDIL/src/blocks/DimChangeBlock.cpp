#include "GIDL/blocks/DimChangeBlock.h"

template<class T>
static void copy1Dim(const SensorValueNumeric<T> *from,SensorValueNumeric<T> *to,quint32 dim)
{
	QVector<T> vals;
	vals.resize((int)from->packetsCount());
	for(quint32 i=0;i<from->packetsCount();++i)
		vals[i]=from->get(dim,i);
	to->setData(vals);
}

DimChangeBlock::DimChangeBlock(quint32 dim,quint32 bId)
	:BaseBlock(bId)
{
	in=new BlockInput(this,DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,1);
	out=new BlockOutput(DataUnit::SINGLE,1);
	outputs.append(out);
	mDim=dim;
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
	else if(in->data().numType()==DataUnit::U64)
		copy1Dim((const SensorValueU64*)in->data().value(),(SensorValueU64*)v.data(),mDim);
	else if(in->data().numType()==DataUnit::F32)
		copy1Dim((const SensorValueF32*)in->data().value(),(SensorValueF32*)v.data(),mDim);
	else if(in->data().numType()==DataUnit::F64)
		copy1Dim((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data(),mDim);
	else return;
	out->setData(DataUnit(v.data()));
}

void DimChangeBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	delete out;
	out=new BlockOutput(in->currentType(),in->currentDim());
	outputs[0]=out;
}
