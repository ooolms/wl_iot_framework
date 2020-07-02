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

#include "VDIL/blocks/AverageCalcBlock.h"
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/blocks/DimChangeBlock.h"
#include "VDIL/blocks/ConditionTransactionBlock.h"
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
	addEditor(ConditionTransactionBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/condition_transaction.png"),"condition transaction",
		"condit. trans.","Condition transaction","condition transaction"));
	addEditor(NormingBlock::mBlockName,new NormingBlockEditor);
	addEditor(DelayBlock::mBlockName,new DelayBlockEditor);
	addEditor(DebugBlock::mBlockName,new DebugBlockEditor);
	addEditor(DateTimeSourceBlock::mBlockName,new DateTimeSourceBlockEditor);
	addEditor(ArrayCombineBlock::mBlockName,new ArrayCombineBlockEditor);
	addEditor(ArraySelectBlock::mBlockName,new ArraySelectBlockEditor);
	addEditor(RandomSourceBlock::mBlockName,new RandomSourceBlockEditor);
	addEditor(DevicePresenceSourceBlock::mBlockName,new DevicePresenceSourceBlockEditor);
}

QString CoreBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	 return "Core blocks";
}