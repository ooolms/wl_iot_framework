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

#include "CoreBlocksGroupEditorsFactory.h"
#include "block_editors/CommandBlockEditor.h"
#include "block_editors/ComparationBlockEditor.h"
#include "block_editors/DebugBlockEditor.h"
#include "block_editors/DefaultBlockEditor.h"
#include "block_editors/DimChangeBlockEditor.h"
#include "block_editors/NormingBlockEditor.h"
#include "block_editors/StaticSourceBlockEditor.h"
#include "block_editors/StorageSourceBlockEditor.h"
#include "block_editors/DelayBlockEditor.h"
#include "block_editors/TimerBlockEditor.h"
#include "block_editors/DeviceStateSourceBlockEditor.h"
#include "block_editors/DateTimeSourceBlockEditor.h"
#include "block_editors/ArrayCombineBlockEditor.h"
#include "block_editors/ArraySelectBlockEditor.h"
#include "block_editors/RandomSourceBlockEditor.h"
#include "block_editors/DevicePresenceSourceBlockEditor.h"
#include "block_editors/RuntimeSourceBlockEditor.h"
#include "block_editors/RuntimeStoreBlockEditor.h"
#include "block_editors/VDevCommandSourceBlockEditor.h"
#include "block_editors/VDevSensorSendBlockEditor.h"
#include "block_editors/MathExpBlockEditor.h"
#include "block_editors/SubProgramBlockEditor.h"
#include "block_editors/SubProgramFakeBlocksEditors.h"
#include "block_editors/TransitionBlockEditor.h"

#include "VDIL/blocks/AverageCalcBlock.h"
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/blocks/DimChangeBlock.h"
#include "VDIL/blocks/ConditionTransitionBlock.h"
#include "VDIL/blocks/NormingBlock.h"
#include "VDIL/blocks/StaticSourceBlock.h"
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/blocks/DebugBlock.h"
#include "VDIL/blocks/DelayBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/blocks/DateTimeSourceBlock.h"
#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/core/TimerBlock.h"
#include "VDIL/blocks/RandomSourceBlock.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/blocks/RuntimeSourceBlock.h"
#include "VDIL/blocks/RuntimeStoreBlock.h"
#include "VDIL/blocks/VDevCommandSourceBlock.h"
#include "VDIL/blocks/VDevSensorSendBlock.h"
#include "VDIL/blocks/MathExpBlock.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/blocks/TransitionBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

CoreBlocksGroupEditorsFactory::CoreBlocksGroupEditorsFactory()
{
	addEditor(CommandBlock::mBlockName,new CommandBlockEditor);
	addEditor(StaticSourceBlock::mBlockName,new StaticSourceBlockEditor);
	addEditor(StorageSourceBlock::mBlockName,new StorageSourceBlockEditor);
	addEditor(DeviceStateSourceBlock::mBlockName,new DeviceStateSourceBlockEditor);
	addEditor(TimerBlock::mBlockName,new TimerBlockEditor);
	addEditor(AverageCalcBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/average_calc.png"),"average calculation",
		"average calc","Average calculation","average calculation"));
	addEditor(ComparationBlock::mBlockName,new ComparationBlockEditor);
	addEditor(DimChangeBlock::mBlockName,new DimChangeBlockEditor);
	addEditor(ConditionTransitionBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/condition_transition.png"),"condition transition",
		"condit. trans.","Condition transition","condition transition"));
	addEditor(NormingBlock::mBlockName,new NormingBlockEditor);
	addEditor(DelayBlock::mBlockName,new DelayBlockEditor);
	addEditor(DebugBlock::mBlockName,new DebugBlockEditor);
	addEditor(DateTimeSourceBlock::mBlockName,new DateTimeSourceBlockEditor);
	addEditor(ArrayCombineBlock::mBlockName,new ArrayCombineBlockEditor);
	addEditor(ArraySelectBlock::mBlockName,new ArraySelectBlockEditor);
	addEditor(RandomSourceBlock::mBlockName,new RandomSourceBlockEditor);
	addEditor(DevicePresenceSourceBlock::mBlockName,new DevicePresenceSourceBlockEditor);
	addEditor(RuntimeSourceBlock::mBlockName,new RuntimeSourceBlockEditor);
	addEditor(RuntimeStoreBlock::mBlockName,new RuntimeStoreBlockEditor);
	addEditor(VDevSensorSendBlock::mBlockName,new VDevSensorSendBlockEditor);
	addEditor(VDevCommandSourceBlock::mBlockName,new VDevCommandSourceBlockEditor);
	addEditor(MathExpBlock::mBlockName,new MathExpBlockEditor);
	addEditor(SubProgramBlock::mBlockName,new SubProgramBlockEditor);
	addEditor(SubProgramInternalInputsFakeBlock::mBlockName,new SubProgramInternalInputsFakeBlockEditor);
	addEditor(SubProgramInternalOutputsFakeBlock::mBlockName,new SubProgramInternalOutputsFakeBlockEditor);
	addEditor(TransitionBlock::mBlockName,new TransitionBlockEditor);
}

QString CoreBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	 return "Core blocks";
}
