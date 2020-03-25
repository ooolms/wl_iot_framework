#include "GIDL/blocks/AverageCalcBlock.h"

template<class T>
static void avCalc(const SensorValueNumeric<T> *in,SensorValueNumeric<T> *out)
{
	quint32 count=in->packetsCount();
	quint32 dim=in->type().dim;
	QVector<T> av;
	av.resize(dim);
	for(quint32 i=0;i<count;++i)
		for(quint32 j=0;j<dim;++j)
			av[j]+=in->valueToDouble(j,i);
	for(quint32 i=0;i<dim;++i)
		av[i]/=count;
	out->setData(av);
}

AverageCalcBlock::AverageCalcBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=new BlockInput(this,DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,0);
	out=new BlockOutput(DataUnit::SINGLE,1);
	outputs.append(out);
}

void AverageCalcBlock::eval()
{
	if(in->data().type()==DataUnit::SINGLE)
		out->setData(DataUnit(in->data().value()));
	else if(in->data().type()==DataUnit::ARRAY&&in->data().value()->packetsCount()>0)
	{
		SensorDef::Type t=in->data().value()->type();
		t.packType=SensorDef::SINGLE;
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(in->data().numType()==DataUnit::F32)
			avCalc((const SensorValueF32*)in->data().value(),(SensorValueF32*)v.data());
		else if(in->data().numType()==DataUnit::S64)
			avCalc((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data());
		else if(in->data().numType()==DataUnit::U64)
			avCalc((const SensorValueU64*)in->data().value(),(SensorValueU64*)v.data());
		else if(in->data().numType()==DataUnit::F64)
			avCalc((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data());
		else return;
		out->setData(DataUnit(v.data()));
	}
}

void AverageCalcBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	delete out;
	out=new BlockOutput(DataUnit::SINGLE,in->currentDim());
	outputs[0]=out;
}
