#include "GDIL/blocks/ComparationBlock.h"
#include <math.h>

const QUuid ComparationBlock::mTypeId=QUuid("{e461bb52-df53-4af4-b307-07ee9acd715c}");

template<class T>
static bool compareValues(T val1,T val2,ComparationBlock::Operation op)
{
	if(op==ComparationBlock::NEQ)
		return val1!=val2;
	else if(op==ComparationBlock::GT)
		return val1>val2;
	else if(op==ComparationBlock::LT)
		return val1<val2;
	else if(op==ComparationBlock::GTEQ)
		return val1>=val2;
	else if(op==ComparationBlock::LTEQ)
		return val1<=val2;
	else
		return val1==val2;
}

ComparationBlock::ComparationBlock(quint32 bId)
	:BaseBlock(bId)
	,mDistValue(1LL)
{
	mBoolOutMode=true;
	mOp=EQ;
	mDimIndex=0;
	in1=new BlockInput(this,DataUnit::SINGLE,DataUnit::SINGLE,0,"value");
	in2=new BlockInput(this,DataUnit::SINGLE,DataUnit::SINGLE,1,"compare with");
	inputs.append(in1);
	inputs.append(in2);
	mkOutputs();
}

void ComparationBlock::setParams(bool boolOutMode,quint32 dimIndex,ComparationBlock::Operation op)
{
	mDimIndex=dimIndex;
	mOp=op;
	if(mBoolOutMode!=boolOutMode)
	{
		rmOutputs();
		mBoolOutMode=boolOutMode;
		mkOutputs();
	}
	updateHint();
}

void ComparationBlock::setDistValue(double val)
{
	mDistValue=DataUnit(val);
}

void ComparationBlock::setDistValue(qint64 val)
{
	mDistValue=DataUnit(val);
}

void ComparationBlock::setDistValue(const DataUnit &val)
{
	if(val.dim()==1&&val.type()==DataUnit::SINGLE)
		mDistValue=val;
}

QUuid ComparationBlock::typeId()const
{
	return mTypeId;
}

bool ComparationBlock::boolOutMode()const
{
	return mBoolOutMode;
}

const DataUnit &ComparationBlock::distValue() const
{
	return mDistValue;
}

quint32 ComparationBlock::dimIndex() const
{
	return mDimIndex;
}

ComparationBlock::Operation ComparationBlock::operation() const
{
	return mOp;
}

void ComparationBlock::eval()
{
	bool compResult=false;
	const SensorValue *val1=in1->data().value();
	const SensorValue *val2=in2->data().value();
	if(mDimIndex>=val1->type().dim)
		return;
	if(mOp==DISTANCE)
	{
		if(in1->data().numType()==DataUnit::F64||in2->data().numType()==DataUnit::F64)
			compResult=fabs(val1->valueToDouble(mDimIndex)-val2->valueToDouble(0))<mDistValue.value()->valueToDouble(0);
		else compResult=abs(val1->valueToS64(mDimIndex)-val2->valueToS64(0))<mDistValue.value()->valueToS64(0);
	}
	else
	{
		if(in1->data().numType()==DataUnit::F64||in2->data().numType()==DataUnit::F64)
			compResult=compareValues(val1->valueToDouble(mDimIndex),val2->valueToDouble(0),mOp);
		else compResult=compareValues(val1->valueToS64(mDimIndex),val2->valueToS64(0),mOp);
	}
	if(mBoolOutMode)
		out1->setData(DataUnit(compResult));
	else if(compResult)
		out1->setData(in1->data());
	else
		out2->setData(in1->data());
}

void ComparationBlock::rmOutputs()
{
	outputs.clear();
	delete out1;
	if(!mBoolOutMode)
		delete out2;
}

void ComparationBlock::mkOutputs()
{
	if(mBoolOutMode)
	{
		out1=new BlockOutput(this,DataUnit::BOOL,1,"bool out");
		outputs.append(out1);
	}
	else
	{
		out1=new BlockOutput(this,in1->type(),in1->dim(),"out if true");
		out2=new BlockOutput(this,in1->type(),in1->dim(),"out if false");
		outputs.append(out1);
		outputs.append(out2);
	}
}

void ComparationBlock::updateHint()
{
	if(mOp==EQ)
		hint="v1==v2";
	else if(mOp==NEQ)
		hint="v1!=v2";
	else if(mOp==GT)
		hint="v1>v2";
	else if(mOp==LT)
		hint="v1<v2";
	else if(mOp==GTEQ)
		hint="v1>=v2";
	else if(mOp==LTEQ)
		hint="v1<=v2";
	else hint="|v1-v2|<"+QByteArray::number(mDistValue.value()->valueToDouble(0));
	if(mBoolOutMode)
		hint+=", boolean output";
	else hint+=", splitted transition of input value";
}
