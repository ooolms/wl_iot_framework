/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "GDIL/blocks/ComparationBlock.h"
#include <qmath.h>
#include "GDIL/core/CoreBlocksGroupFactory.h"

const QString ComparationBlock::mBlockName=QString("comparation");

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
	mExternalV2Input=true;
	mOutMode=SINGLE_BOOL;
	mOp=EQ;
	mDimIndex=0;
	in1=mkInput(TypeConstraints(DataUnit::SINGLE,0),DataUnit::SINGLE,"v1");
	in2=mkInput(TypeConstraints(DataUnit::SINGLE,1),DataUnit::SINGLE,"v2");
	out1=mkOutput(DataUnit::BOOL,1,"bool out");
	mV2Value=DataUnit(0.0);
}

QString ComparationBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString ComparationBlock::blockName()const
{
	return mBlockName;
}

void ComparationBlock::setParams(OutMode outMode,bool externalV2Input,quint32 dimIndex,ComparationBlock::Operation op)
{
	mDimIndex=dimIndex;
	mOp=op;

	if(mExternalV2Input!=externalV2Input)
	{
		mExternalV2Input=externalV2Input;
		if(mExternalV2Input)
			in2=mkInput(TypeConstraints(DataUnit::SINGLE,1),DataUnit::SINGLE,"v2");
		else
		{
			rmInput(in2);
			in2=0;
		}
	}

	if(mOutMode!=outMode)
	{
		if(mOutMode==SINGLE_BOOL)//was 1 out, now 2
			out2=mkOutput(DataUnit::BOOL,1,"out2");
		else if(outMode==SINGLE_BOOL)//was 2 outputs, now 1
		{
			rmOutput(out2);
			out2=0;
		}

		mOutMode=outMode;

		if(mOutMode==SINGLE_BOOL)
		{
			out1->replaceTypeAndDim(DataUnit::BOOL,1);
			out1->setTitle("bool out");
		}
		else if(mOutMode==SPLITTED_BOOL)
		{
			out1->replaceTypeAndDim(DataUnit::BOOL,1);
			out1->setTitle("true out");
			out2->replaceTypeAndDim(DataUnit::BOOL,1);
			out2->setTitle("false out");
		}
		else
		{
			out1->replaceTypeAndDim(in1->type(),in1->dim());
			out1->setTitle("v1 out if true");
			out2->replaceTypeAndDim(in1->type(),in1->dim());
			out2->setTitle("v1 out if false");
		}
	}
}

void ComparationBlock::setDistValue(const DataUnit &val)
{
	if(val.dim()==1&&val.type()==DataUnit::SINGLE)
		mDistValue=val;
}

void ComparationBlock::setV2Value(const DataUnit &val)
{
	mV2Value=val;
}

ComparationBlock::OutMode ComparationBlock::outMode()const
{
	return mOutMode;
}

const DataUnit& ComparationBlock::distValue()const
{
	return mDistValue;
}

quint32 ComparationBlock::dimIndex()const
{
	return mDimIndex;
}

ComparationBlock::Operation ComparationBlock::operation()const
{
	return mOp;
}

bool ComparationBlock::externalV2Input()const
{
	return mExternalV2Input;
}

const DataUnit& ComparationBlock::v2Value()const
{
	return mV2Value;
}

void ComparationBlock::eval()
{
	bool compResult=false;
	const SensorValue *val1=in1->data().value();
	const SensorValue *val2;
	if(mExternalV2Input)
		val2=in2->data().value();
	else val2=mV2Value.value();
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
	if(mOutMode==SINGLE_BOOL)
		out1->setData(DataUnit(compResult));
	else if(mOutMode==SPLITTED_BOOL)
	{
		if(compResult)
			out1->setData(DataUnit(compResult));
		else
			out2->setData(DataUnit(compResult));
	}
	else
	{
		if(compResult)
			out1->setData(in1->data());
		else
			out2->setData(in1->data());
	}
}

void ComparationBlock::onInputTypeSelected(BlockInput *b)
{
	if(b==in1&&mOutMode==SPLITTED_INPUT)
	{
		out1->replaceTypeAndDim(in1->type(),in1->dim());
		out2->replaceTypeAndDim(in1->type(),in1->dim());
	}
}
