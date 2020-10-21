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

#include "VDIL/core/ProgramStoragesRuntime.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

ProgramStoragesRuntimeInstance::ProgramStoragesRuntimeInstance(WLIOTVDIL::Program *p)
	:IProgramRuntimeInstance(p)
{
}

void ProgramStoragesRuntimeInstance::prepareToStart()
{
	for(BaseBlock *b:prg->allBlocks())
	{
		if(b->groupName()!=CoreBlocksGroupFactory::mGroupName)continue;
		if(b->blockName()==StorageSourceBlock::mBlockName)
		{
			StorageSourceBlock *bb=(StorageSourceBlock*)b;
			if(!bb->useTrigger())continue;
			StorageId id=bb->storageId();
			if(id.deviceId.isNull()||id.sensorName.isEmpty())continue;
			mBlocksMap[id].append(bb);
			if(mConnMap.contains(id))continue;
			ISensorStorage *st=prg->helper()->storageById(id);
			if(!st)continue;
			mConnMap[id]=connect(st,&ISensorStorage::newValueWritten,
				this,&ProgramStoragesRuntimeInstance::onNewValue);
		}
	}
}

void ProgramStoragesRuntimeInstance::cleanupAfterStop()
{
	for(QMetaObject::Connection &c:mConnMap)
		disconnect(c);
	mConnMap.clear();
	mBlocksMap.clear();
}

void ProgramStoragesRuntimeInstance::onNewValue()
{
	ISensorStorage *st=(ISensorStorage*)sender();
	for(StorageSourceBlock *b:mBlocksMap.value({st->deviceId(),st->sensor().name}))
		b->setTriggerActivated();
	emit activateProgram();
}

IProgramRuntimeInstance* ProgramStoragesRuntime::mkRuntime(Program *p)
{
	return new ProgramStoragesRuntimeInstance(p);
}
