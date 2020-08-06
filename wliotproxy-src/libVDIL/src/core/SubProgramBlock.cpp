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

const QString SubProgramInternalInputsFakeBlock::mBlockName="subprogram_fake_internal_inputs";
const QString SubProgramInternalOutputsFakeBlock::mBlockName="subprogram_fake_internal_outputs";
const QString SubProgramBlock::mBlockName=QString("subprogram");

SubProgramInternalOutputsFakeBlock::SubProgramInternalOutputsFakeBlock()
	:BaseBlock(0)
{
	title="start";
	position=QPointF(0,0);
}

SubProgramInternalInputsFakeBlock::SubProgramInternalInputsFakeBlock()
	:BaseBlock(0)
{
	title="end";
	position=QPointF(200,0);
}

QString SubProgramInternalOutputsFakeBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString SubProgramInternalOutputsFakeBlock::blockName()const
{
	return mBlockName;
}

void SubProgramInternalOutputsFakeBlock::updateOutput(int index,const TypeAndDim &type,const QString &title)
{
	BlockOutput *o=output(index);
	o->replaceTypeAndDim(type);
	o->setTitle(title);
}

void SubProgramInternalOutputsFakeBlock::eval()
{
}

QString SubProgramInternalInputsFakeBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString SubProgramInternalInputsFakeBlock::blockName()const
{
	return mBlockName;
}

void SubProgramInternalInputsFakeBlock::updateInput(int index,const TypeAndDim &t,const QString &tt)
{
	BlockInput *in=input(index);
	in->replaceTypesAndDim(t);
	in->setTitle(tt);
}

void SubProgramInternalInputsFakeBlock::eval()
{
}

SubProgramBlock::SubProgramBlock(quint32 blockId)
	:BaseBlock(blockId)
{
	sprg=new SubProgram;
	mInternalInputsBlock=new SubProgramInternalInputsFakeBlock();
	mInternalOutputsBlock=new SubProgramInternalOutputsFakeBlock();
}

SubProgramBlock::~SubProgramBlock()
{
	delete mInternalInputsBlock;
	delete mInternalOutputsBlock;
}

void SubProgramBlock::setParams(const QList<TypeAndDim> &inputTypes,
	const QStringList &inputTitles,const QList<TypeAndDim> &outputTypes,const QStringList &outputTitles)
{
	if(inputTypes.count()!=inputTitles.count())return;
	if(outputTypes.count()!=outputTitles.count())return;
	//inputs
	for(int i=0;i<inputTypes.count();++i)
	{
		const TypeAndDim &iType=inputTypes[i];
		const QString &iTitle=inputTitles[i];
		if(i>=mArgInputs.count())
		{
			mArgInputs.append(mkInput(iType,iTitle));
			mInternalOutputsBlock->mkOutput(iType,iTitle);
		}
		else
		{
			mArgInputs[i]->replaceTypesAndDim(iType);
			mArgInputs[i]->setTitle(iTitle);
			mInternalOutputsBlock->updateOutput(i,iType,iTitle);
		}
	}
	while(mArgInputs.count()>inputTypes.count())
	{
		rmInput(mArgInputs.takeLast());
		mInternalOutputsBlock->rmOutput(mArgInputs.count());
	}
	//outputs
	for(int i=0;i<outputTypes.count();++i)
	{
		const TypeAndDim &iType=outputTypes[i];
		const QString &iTitle=outputTitles[i];
		if(i>=mArgOutputs.count())
		{
			mArgOutputs.append(mkOutput(iType,iTitle));
			mInternalInputsBlock->mkInput(iType,iTitle);
		}
		else
		{
			mArgOutputs[i]->replaceTypeAndDim(iType);
			mArgOutputs[i]->setTitle(iTitle);
			mInternalInputsBlock->updateInput(i,iType,iTitle);
		}
	}
	while(mArgOutputs.count()>outputTypes.count())
	{
		rmOutput(mArgOutputs.takeLast());
		mInternalInputsBlock->rmInput(mArgOutputs.count());
	}
}

QList<TypeAndDim> SubProgramBlock::inputTypes()const
{
	QList<TypeAndDim> r;
	for(const BlockInput *i:mArgInputs)
		r.append(i->type());
	return r;
}

QStringList SubProgramBlock::inputTitles()const
{
	QStringList r;
	for(const BlockInput *i:mArgInputs)
		r.append(i->title());
	return r;
}

QList<TypeAndDim> SubProgramBlock::outputTypes()const
{
	QList<TypeAndDim> r;
	for(const BlockOutput *o:mArgOutputs)
		r.append(o->type());
	return r;
}

QStringList SubProgramBlock::outputTitles()const
{
	QStringList r;
	for(const BlockOutput *o:mArgOutputs)
		r.append(o->title());
	return r;
}

SubProgramInternalOutputsFakeBlock* SubProgramBlock::internalOutputsBlock()
{
	return mInternalOutputsBlock;
}

const SubProgramInternalOutputsFakeBlock* SubProgramBlock::internalOutputsBlock()const
{
	return mInternalOutputsBlock;
}

SubProgramInternalInputsFakeBlock* SubProgramBlock::internalInputsBlock()
{
	return mInternalInputsBlock;
}

const SubProgramInternalInputsFakeBlock* SubProgramBlock::internalInputsBlock()const
{
	return mInternalInputsBlock;
}

int SubProgramBlock::argInputsCount()const
{
	return mArgInputs.count();
}

BlockInput* SubProgramBlock::argInput(int index)
{
	return mArgInputs.value(index);
}

const BlockInput *SubProgramBlock::argInput(int index)const
{
	return mArgInputs.value(index);
}

int SubProgramBlock::argOutputsCount()const
{
	return mArgOutputs.count();
}

BlockOutput* SubProgramBlock::argOutput(int index)
{
	return mArgOutputs.value(index);
}

const BlockOutput* SubProgramBlock::argOutput(int index)const
{
	return mArgOutputs.value(index);
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

void SubProgramBlock::eval()
{
	for(int i=0;i<mArgInputs.count();++i)
		mInternalOutputsBlock->output(i)->setData(mArgInputs[i]->data());
	subPrg->evalTimers()->waitForTimers();
	for(int i=0;i<mArgOutputs.count();++i)
	{
		BlockInput *in=mInternalInputsBlock->input(i);
		if(in->isReady())
			mArgOutputs[i]->setData(in->data());
	}
}

void WLIOTVDIL::SubProgramBlock::cleanupAfterEvalInternal()
{
	mInternalInputsBlock->cleanupAfterEval();
}
