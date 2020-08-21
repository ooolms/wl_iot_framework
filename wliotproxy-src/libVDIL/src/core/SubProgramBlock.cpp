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
#include "VDIL/core/CoreBlocksGroupFactory.h"

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
	return CoreBlocksGroupFactory::mGroupName;
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

void SubProgramBlock::updateInputsOutputs()
{
	QMap<QString,TypeAndDim> inputs=sprg->inputs();
	QMap<QString,TypeAndDim> outputs=sprg->outputs();
	//inputs
	for(QString &k:mArgInputs.keys())
	{
		if(!inputs.contains(k))
			rmInput(mArgInputs.take(k));
	}
	for(auto i=inputs.begin();i!=inputs.end();++i)
	{
		BlockInput *in=mArgInputs.value(i.key());
		if(!in)mArgInputs[i.key()]=mkInput(i.value(),i.key());
		else in->replaceTypesAndDim(i.value());
	}
	//outputs
	for(QString &k:mArgOutputs.keys())
	{
		if(!outputs.contains(k))
			rmOutput(mArgOutputs.take(k));
	}
	for(auto i=outputs.begin();i!=outputs.end();++i)
	{
		BlockOutput *out=mArgOutputs.value(i.key());
		if(!out)mArgOutputs[i.key()]=mkOutput(i.value(),i.key());
		else out->replaceTypeAndDim(i.value());
	}
}

void SubProgramBlock::eval()
{
	for(BlockInput *in:mArgInputs)
		sprg->setInputData(in->title(),in->data());
	sprg->evalSubProgram();
	const QMap<QString,DataUnit> &outData=sprg->outputsData();
	for(auto i=outData.begin();i!=outData.end();++i)
	{
		if(!i.value().isValid())continue;
		BlockOutput *out=mArgOutputs.value(i.key());
		if(!out)continue;
		out->setData(i.value());
	}
}
