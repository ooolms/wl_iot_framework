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

#include "VDIL/core/SubProgram.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramEvalTimers.h"

using namespace WLIOTVDIL;

const QString SubProgramInternalInputBlock::mBlockName="subprogram_internal_input";
const QString SubProgramInternalOutputBlock::mBlockName="subprogram_internal_output";

SubProgram::SubProgram()
{
	prg=0;
	mEvalTimers=new ProgramEvalTimers;
}

SubProgram::~SubProgram()
{
	delete mEvalTimers;
}

const QMap<quint32,BaseBlock*>& SubProgram::selfBlocks()const
{
	return mSelfBlocks;
}

bool SubProgram::addBlock(BaseBlock *b)
{
	if(!prg)return false;
	if(!prg->addBlockFromSubProgram(b))return false;
	mSelfBlocks[b->blockId()]=b;
	if(b->isSourceBlock())
		mSelfSourceBlocks[b->blockId()]=(SourceBlock*)b;
	b->ownerSubPrg=this;
	b->onProgramIsSet();
	return true;
}

void SubProgram::rmBlock(quint32 bId)
{
	if(!prg)return;
	mSelfSourceBlocks.remove(bId);
	mSelfBlocks.remove(bId);
	prg->rmBlockFromSubProgram(bId);
}

void SubProgram::setInputs(const QMap<QString,TypeAndDim> &inputs)
{
	mInputs=inputs;
	mInputs.remove(QString());
	for(BaseBlock *b:mSelfBlocks)
	{
		if(b->groupName()==Program::reservedCoreGroupName&&
			b->blockName()==SubProgramInternalOutputBlock::mBlockName)
		{
			SubProgramInternalOutputBlock *bb=(SubProgramInternalOutputBlock*)b;
			if(!mInputs.contains(bb->subProgramInput()))
				bb->setSubProgramInput("",TypeAndDim());
		}
	}
}

void SubProgram::setOutputs(const QMap<QString,TypeAndDim> &outputs)
{
	mOutputs=outputs;
	mOutputs.remove(QString());
	for(BaseBlock *b:mSelfBlocks)
	{
		if(b->groupName()==Program::reservedCoreGroupName&&
			b->blockName()==SubProgramInternalInputBlock::mBlockName)
		{
			SubProgramInternalInputBlock *bb=(SubProgramInternalInputBlock*)b;
			if(!mInputs.contains(bb->subProgramOutput()))
				bb->setSubProgramOutput("",TypeAndDim());
		}
	}
}

const QMap<QString,TypeAndDim>& SubProgram::inputs()const
{
	return mInputs;
}

const QMap<QString,TypeAndDim>& SubProgram::outputs()const
{
	return mOutputs;
}

void SubProgram::evalSubProgram()
{
	mOutputsData.clear();
	for(auto i=mSelfSourceBlocks.begin();i!=mSelfSourceBlocks.end();++i)
		i.value()->evalIfReady();
	for(auto i=mInternalOutputBlocks.begin();i!=mInternalOutputBlocks.end();++i)
	{
		DataUnit v=mInputsData.value(i.key());
		if(!v.isValid())continue;
		for(SubProgramInternalOutputBlock *b:i.value())
		{
			b->setData(v);
			b->evalIfReady();
		}
	}
	mEvalTimers->waitForTimers();
	mInputsData.clear();
}

void SubProgram::setInputData(const QString &input,const DataUnit &d)
{
	if(mInputs.contains(input))
		mInputsData[input]=d;
}

const QMap<QString,DataUnit>& SubProgram::outputsData()
{
	return mOutputsData;
}

void SubProgram::prepareSubProgramToStart()
{
	for(BaseBlock *b:mSelfBlocks)
	{
		if(b->groupName()==Program::reservedCoreGroupName&&
			b->blockName()==SubProgramInternalOutputBlock::mBlockName)
		{
			SubProgramInternalOutputBlock *bb=(SubProgramInternalOutputBlock*)b;
			if(mInputs.contains(bb->subProgramInput()))
				mInternalOutputBlocks[bb->subProgramInput()].append(bb);
		}
	}
}

void SubProgram::cleanupSubProgramAfterStop()
{
	mInternalOutputBlocks.clear();
}

ProgramEvalTimers* SubProgram::evalTimers()
{
	return mEvalTimers;
}

SubProgramInternalOutputBlock::SubProgramInternalOutputBlock(quint32 bId)
	:BaseBlock(bId)
{
	out=0;
}

QString SubProgramInternalOutputBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString SubProgramInternalOutputBlock::blockName()const
{
	return mBlockName;
}

void SubProgramInternalOutputBlock::setSubProgramInput(const QString &input,const TypeAndDim &type)
{
	mSubProgramInput=input;
	if(input.isEmpty()||!type.isValid())
	{
		if(out)
		{
			rmOutput(out);
			out=0;
		}
	}
	else if(out)
	{
		out->setTitle(input);
		out->replaceTypeAndDim(type);
	}
	else out=mkOutput(type,title);
}

const QString& SubProgramInternalOutputBlock::subProgramInput()const
{
	return mSubProgramInput;
}

void SubProgramInternalOutputBlock::setData(const DataUnit &u)
{
	mData=u;
}

void SubProgramInternalOutputBlock::eval()
{
	if(out)
		out->setData(mData);
}

SubProgramInternalInputBlock::SubProgramInternalInputBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=0;
}

QString SubProgramInternalInputBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString SubProgramInternalInputBlock::blockName()const
{
	return mBlockName;
}

void SubProgramInternalInputBlock::setSubProgramOutput(const QString &output,const TypeAndDim &type)
{
	mSubProgramOutput=output;
	if(output.isEmpty()||!type.isValid())
	{
		if(in)
		{
			rmInput(in);
			in=0;
		}
	}
	else if(in)
	{
		in->setTitle(output);
		in->replaceTypesAndDim(type);
	}
	else in=mkInput(type,title);
}

const QString& SubProgramInternalInputBlock::subProgramOutput()const
{
	return mSubProgramOutput;
}

void SubProgramInternalInputBlock::eval()
{
	if(in&&ownerSubPrg)
		ownerSubPrg->mOutputsData[mSubProgramOutput]=in->data();
}
