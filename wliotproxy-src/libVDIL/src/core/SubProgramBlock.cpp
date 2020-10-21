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

#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/SubProgram.h"
#include "VDIL/core/ProgramEvalTimers.h"
#include "VDIL/core/Program.h"

using namespace WLIOTVDIL;

const QString SubProgramBlock::mBlockName=QString("subprogram");

SubProgramBlock::SubProgramBlock(quint32 blockId)
	:BaseBlock(blockId)
{
	sprg=new SubProgram;
}

SubProgramBlock::~SubProgramBlock()
{
	delete sprg;
}

void SubProgramBlock::onProgramIsSet()
{
	sprg->prg=prg;
}

QString SubProgramBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString SubProgramBlock::blockName()const
{
	return mBlockName;
}

SubProgram* SubProgramBlock::subProgram()
{
	return sprg;
}

const SubProgram *SubProgramBlock::subProgram()const
{
	return sprg;
}

bool SubProgramBlock::setupInputsOutputs(const QStringList &savedInputsOrder,const QStringList &savedOutputsOrder)
{
	if(!mArgInputs.isEmpty()||!mArgOutputs.isEmpty())
		return false;
	for(const QString &s:savedInputsOrder)
		if(!sprg->inputs().contains(s))
			return false;
	for(const QString &s:savedOutputsOrder)
		if(!sprg->outputs().contains(s))
			return false;
	for(const QString &s:savedInputsOrder)
		mArgInputs.append(mkInput(sprg->inputs()[s],s));
	for(const QString &s:savedOutputsOrder)
		mArgOutputs.append(mkOutput(sprg->outputs()[s],s));
	return true;
}

void SubProgramBlock::updateInputsOutputs()
{
	QMap<QString,TypeAndDim> inputs=sprg->inputs();
	QMap<QString,TypeAndDim> outputs=sprg->outputs();
	//inputs
	for(int i=0;i<mArgInputs.count();++i)
	{
		if(!inputs.contains(mArgInputs[i]->title()))
		{
			rmInput(mArgInputs.takeAt(i));
			--i;
		}
	}
	for(auto i=inputs.begin();i!=inputs.end();++i)
	{
		int index=inputIndex(i.key());
		if(index==-1)
			mArgInputs.append(mkInput(i.value(),i.key()));
		else mArgInputs[index]->replaceTypesAndDim(i.value());
	}
	//outputs
	for(int i=0;i<mArgOutputs.count();++i)
	{
		if(!outputs.contains(mArgOutputs[i]->title()))
		{
			rmOutput(mArgOutputs.takeAt(i));
			--i;
		}
	}
	for(auto i=outputs.begin();i!=outputs.end();++i)
	{
		int index=outputIndex(i.key());
		if(index==-1)
			mArgOutputs.append(mkOutput(i.value(),i.key()));
		else mArgOutputs[index]->replaceTypeAndDim(i.value());
	}
}

QStringList SubProgramBlock::inputsOrder()
{
	QStringList r;
	for(BlockInput *in:mArgInputs)
		r.append(in->title());
	return r;
}

QStringList SubProgramBlock::outputsOrder()
{
	QStringList r;
	for(BlockOutput *out:mArgOutputs)
		r.append(out->title());
	return r;
}

void SubProgramBlock::eval()
{
	for(BlockInput *in:mArgInputs)
		sprg->setInputData(in->title(),in->data());
	sprg->evalSubProgram();
	const QMap<QString,DataUnit> &outData=sprg->outputsData();
	for(BlockOutput *out:mArgOutputs)
	{
		DataUnit v=outData.value(out->title());
		if(v.isValid())out->setData(v);
	}
}

int SubProgramBlock::inputIndex(const QString &input)
{
	for(int i=0;i<mArgInputs.count();++i)
		if(mArgInputs[i]->title()==input)
			return i;
	return -1;
}

int SubProgramBlock::outputIndex(const QString &output)
{
	for(int i=0;i<mArgOutputs.count();++i)
		if(mArgOutputs[i]->title()==output)
			return i;
	return -1;
}
