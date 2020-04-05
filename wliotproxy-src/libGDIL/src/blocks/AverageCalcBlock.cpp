#include "GDIL/blocks/AverageCalcBlock.h"

const QUuid AverageCalcBlock::mTypeId=QUuid("{fdb44c5f-680c-4469-8b2a-059557be9ca9}");

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
	in=new BlockInput(this,DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,0,"in");
	inputs.append(in);

	out=new BlockOutput(this,DataUnit::SINGLE,1,"average");
	outputs.append(out);
}

QUuid AverageCalcBlock::typeId()const
{
	return mTypeId;
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
		if(in->data().numType()==DataUnit::S64)
			avCalc((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data());
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
	out=new BlockOutput(this,DataUnit::SINGLE,in->dim(),"average");
	outputs[0]=out;
}
