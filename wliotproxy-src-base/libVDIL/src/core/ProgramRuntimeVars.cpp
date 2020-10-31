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

#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/RuntimeSourceBlock.h"
#include "VDIL/core/RuntimeStoreBlock.h"

using namespace WLIOTVDIL;

ProgramRuntimeVars::ProgramRuntimeVars(Program *p)
	:IProgramRuntimeInstance(p)
{
}

void ProgramRuntimeVars::setupVar(const QString &name,const WLIOTVDIL::DataUnit &defaultValue)
{
	if(name.isEmpty())return;
	mDefaultValues[name]=defaultValue;
	mRuntimeValues[name]=defaultValue;
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==RuntimeSourceBlock::mBlockName)
		{
			RuntimeSourceBlock *bb=(RuntimeSourceBlock*)b;
			if(bb->varName()==name)
				bb->setVarName(name);
		}
		else if(b->blockName()==RuntimeStoreBlock::mBlockName)
		{
			RuntimeStoreBlock *bb=(RuntimeStoreBlock*)b;
			if(bb->varName()==name)
				bb->setVarName(name);
		}
	}
}

void ProgramRuntimeVars::removeVar(const QString &name)
{
	mDefaultValues.remove(name);
	mRuntimeValues.remove(name);
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=Program::reservedCoreGroupName)continue;
		if(b->blockName()==RuntimeSourceBlock::mBlockName)
		{
			RuntimeSourceBlock *bb=(RuntimeSourceBlock*)b;
			if(bb->varName()==name)
				bb->setVarName("");
		}
		else if(b->blockName()==RuntimeStoreBlock::mBlockName)
		{
			RuntimeStoreBlock *bb=(RuntimeStoreBlock*)b;
			if(bb->varName()==name)
				bb->setVarName("");
		}
	}
}

DataUnit ProgramRuntimeVars::defaultValue(const QString &name)const
{
	return mDefaultValues.value(name);
}

QStringList ProgramRuntimeVars::allVars()const
{
	return mDefaultValues.keys();
}

DataUnit ProgramRuntimeVars::runtimeValue(const QString &name)const
{
	return mRuntimeValues.value(name);
}

void ProgramRuntimeVars::setRuntimeValue(const QString &name,const DataUnit &value)
{
	if(!mRuntimeValues.contains(name))return;
	DataUnit &v=mRuntimeValues[name];
	if(v.type()!=value.type()||v.dim()!=value.dim())return;
	v=value;
}


void WLIOTVDIL::ProgramRuntimeVars::prepareToStart()
{
	mRuntimeValues=mDefaultValues;
}

void WLIOTVDIL::ProgramRuntimeVars::cleanupAfterStop()
{
}
