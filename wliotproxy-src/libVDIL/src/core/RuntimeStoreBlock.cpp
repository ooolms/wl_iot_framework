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

#include "VDIL/core/RuntimeStoreBlock.h"
#include "VDIL/core/RuntimeSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"

using namespace WLIOTVDIL;

const QString RuntimeStoreBlock::mBlockName=QString("runtime_store");

RuntimeStoreBlock::RuntimeStoreBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=0;
}

QString RuntimeStoreBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString RuntimeStoreBlock::blockName()const
{
	return mBlockName;
}

void RuntimeStoreBlock::setVarName(const QString &varName)
{
	if(in)
	{
		rmInput(in);
		in=0;
	}
	mVarName=varName;
	if(!prg)return;
	DataUnit v=prg->runtimeVars()->defaultValue(mVarName);
	if(v.isValid())
		in=mkInput(TypeConstraints(v.type(),v.dim()),v.type(),"in");
}

QString RuntimeStoreBlock::varName()const
{
	return mVarName;
}

void RuntimeStoreBlock::eval()
{
	prg->runtimeVars()->setRuntimeValue(mVarName,in->data());
}
