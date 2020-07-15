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

#include "VDIL/blocks/RuntimeSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include <stdlib.h>
#include <limits>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString RuntimeSourceBlock::mBlockName=QString("runtime_source");

RuntimeSourceBlock::RuntimeSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	out=0;
}

QString RuntimeSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString RuntimeSourceBlock::blockName()const
{
	return mBlockName;
}

void RuntimeSourceBlock::setVarName(const QString &varName)
{
	if(out)
	{
		rmOutput(out);
		out=0;
	}
	mVarName=varName;
	if(!prg)return;
	DataUnit v=prg->runtimeVars()->defaultValue(varName);
	if(v.isValid())
		out=mkOutput(v.type(),v.dim(),"out");
}

QString RuntimeSourceBlock::varName()const
{
	return mVarName;
}

DataUnit WLIOTVDIL::RuntimeSourceBlock::extractDataInternal()
{
	return prg->runtimeVars()->runtimeValue(mVarName);
}
