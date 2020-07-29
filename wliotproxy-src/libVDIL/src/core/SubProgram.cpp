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
	b->subPrg=this;
	b->onProgramIsSet();
	return true;
}

void SubProgram::rmBlock(quint32 bId)
{
	if(!prg)return;
	mSelfBlocks.remove(bId);
	prg->rmBlockFromSubProgram(bId);
}

ProgramEvalTimers* SubProgram::evalTimers()
{
	return mEvalTimers;
}
