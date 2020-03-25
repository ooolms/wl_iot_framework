#include "GIDL/blocks/NormingBlock.h"

template<class T>
static void normCalc(const SensorValueNumeric<T> *in,SensorValueNumeric<T> *out,
	const T &minX,const T &maxX,const T &minY,const T &maxY)
{
	QVector<T> vals;
	vals.resize(in->data().size());
	for(int i=0;i<in->data().size();++i)
		vals[i]=in->data()[i]*(maxY-minY)/(maxX-minX);
	out->setData(vals);
}

NormingBlock::NormingBlock(quint32 bId,const DataUnit &minX,const DataUnit &maxX,
	const DataUnit &minY,const DataUnit &maxY)
	:BaseBlock(bId)
	,mMinX(minX)
	,mMaxX(maxX)
	,mMinY(minY)
	,mMaxY(maxY)
{
	in=new BlockInput(this,DataUnit::SINGLE|DataUnit::ARRAY,DataUnit::SINGLE,1);
	out=new BlockOutput(DataUnit::SINGLE,1);
	outputs.append(out);
}

void NormingBlock::eval()
{
	SensorDef::Type t=in->data().value()->type();
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
	if(in->data().numType()==DataUnit::S64)
	{
		qint64 minX=mMinX.value()->valueToS64(0);
		qint64 maxX=mMaxX.value()->valueToS64(0);
		qint64 minY=mMinY.value()->valueToS64(0);
		qint64 maxY=mMaxY.value()->valueToS64(0);
		normCalc((const SensorValueS64*)in->data().value(),(SensorValueS64*)v.data(),
			minX,maxX,minY,maxY);
	}
	else if(in->data().numType()==DataUnit::U64)
	{
		quint64 minX=mMinX.value()->valueToU64(0);
		quint64 maxX=mMaxX.value()->valueToU64(0);
		quint64 minY=mMinY.value()->valueToU64(0);
		quint64 maxY=mMaxY.value()->valueToU64(0);
		normCalc((const SensorValueU64*)in->data().value(),(SensorValueU64*)v.data(),
			minX,maxX,minY,maxY);
	}
	else if(in->data().numType()==DataUnit::F32)
	{
		float minX=mMinX.value()->valueToDouble(0);
		float maxX=mMaxX.value()->valueToDouble(0);
		float minY=mMinY.value()->valueToDouble(0);
		float maxY=mMaxY.value()->valueToDouble(0);
		normCalc((const SensorValueF32*)in->data().value(),(SensorValueF32*)v.data(),
			minX,maxX,minY,maxY);
	}
	else if(in->data().numType()==DataUnit::F64)
	{
		double minX=mMinX.value()->valueToDouble(0);
		double maxX=mMaxX.value()->valueToDouble(0);
		double minY=mMinY.value()->valueToDouble(0);
		double maxY=mMaxY.value()->valueToDouble(0);
		normCalc((const SensorValueF64*)in->data().value(),(SensorValueF64*)v.data(),
			minX,maxX,minY,maxY);
	}
	else return;
	out->setData(DataUnit(v.data()));
}

void NormingBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
	delete out;
	out=new BlockOutput(in->currentType(),in->currentDim());
	outputs[0]=out;
}
