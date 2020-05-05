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
#include "GDIL/blocks/AverageCalcBlock.h"
#include "GDIL/blocks/CommandBlock.h"
#include "GDIL/blocks/ComparationBlock.h"
#include "GDIL/blocks/DimChangeBlock.h"
#include "GDIL/blocks/ConditionTransactionBlock.h"
#include "GDIL/blocks/NormingBlock.h"
#include "GDIL/blocks/StaticSourceBlock.h"
#include "GDIL/blocks/StorageSourceBlock.h"
#include "GDIL/blocks/DebugBlock.h"
#include "GDIL/blocks/DelayBlock.h"
#include "GDIL/blocks/DeviceStateSourceBlock.h"
#include "GDIL/core/TimerBlock.h"

CoreBlocksGroupEditorsFactory::CoreBlocksGroupEditorsFactory()
{
	addEditor(CommandBlock::mBlockName,new CommandBlockEditor);
	addEditor(StaticSourceBlock::mBlockName,new StaticSourceBlockEditor);
	addEditor(StorageSourceBlock::mBlockName,new StorageSourceBlockEditor);
	addEditor(DeviceStateSourceBlock::mBlockName,new DeviceStateSourceBlockEditor);
	addEditor(TimerBlock::mBlockName,new TimerBlockEditor);
	addEditor(AverageCalcBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/average_calc.png"),"average calculation","average calc"));
	addEditor(ComparationBlock::mBlockName,new ComparationBlockEditor);
	addEditor(DimChangeBlock::mBlockName,new DimChangeBlockEditor);
	addEditor(ConditionTransactionBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/condition_transaction.png"),"condition transaction","condition transaction"));
	addEditor(NormingBlock::mBlockName,new NormingBlockEditor);
	addEditor(DelayBlock::mBlockName,new DelayBlockEditor);
	addEditor(DebugBlock::mBlockName,new DebugBlockEditor);
}

QString CoreBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	 return "Core blocks";
}
