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

#include "GDIL/editor/BlocksEditingFactory.h"
#include "GDIL/blocks/AllBlocks.h"
#include "block_editors/DefaultBlockEditor.h"
#include "block_editors/CommandBlockEditor.h"
#include "block_editors/ComparationBlockEditor.h"
#include "block_editors/DimChangeBlockEditor.h"
#include "block_editors/NormingBlockEditor.h"
#include "block_editors/StaticSourceBlockEditor.h"
#include "block_editors/StorageSourceBlockEditor.h"
#include "block_editors/DebugBlockEditor.h"

BlocksEditingFactory::BlocksEditingFactory()
{
	editors[AverageCalcBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/average_calc.png"),"average calculation","average calc");
	editors[CommandBlock::mTypeId]=new CommandBlockEditor;
	editors[ComparationBlock::mTypeId]=new ComparationBlockEditor;
	editors[DimChangeBlock::mTypeId]=new DimChangeBlockEditor;
	editors[ConditionTransactionBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/condition_transaction.png"),"condition transaction","condition transaction");
	editors[NotBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_not.png"),"logical NOT","NOT");
	editors[AndBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_and.png"),"logical AND","AND");
	editors[OrBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_or.png"),"logical OR","OR");
	editors[XorBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_xor.png"),"logical XOR","XOR");
	editors[AndNotBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_and_not.png"),"logical AND-NOT","AND-NOT");
	editors[OrNotBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_or_not.png"),"logical OR-NOT","OR-NOT");
	editors[XorNotBoolBlock::mTypeId]=new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_xor_not.png"),"logical XOR-NOT","XOR-NOT");
	editors[NormingBlock::mTypeId]=new NormingBlockEditor;
	editors[StaticSourceBlock::mTypeId]=new StaticSourceBlockEditor;
	editors[StorageSourceBlock::mTypeId]=new StorageSourceBlockEditor;
	editors[DebugBlock::mTypeId]=new DebugBlockEditor;

	types<<DebugBlock::mTypeId<<
		StaticSourceBlock::mTypeId<<
		StorageSourceBlock::mTypeId<<
		CommandBlock::mTypeId<<
		AverageCalcBlock::mTypeId<<
		ComparationBlock::mTypeId<<
		DimChangeBlock::mTypeId<<
		ConditionTransactionBlock::mTypeId<<
		NormingBlock::mTypeId<<
		NotBoolBlock::mTypeId<<
		AndBoolBlock::mTypeId<<
		OrBoolBlock::mTypeId<<
		XorBoolBlock::mTypeId<<
		AndNotBoolBlock::mTypeId<<
		OrNotBoolBlock::mTypeId<<
		XorNotBoolBlock::mTypeId;
}

IBlockEditor* BlocksEditingFactory::editorByType(const QUuid &typeId)
{
	return editors.value(typeId);
}

QList<QUuid> BlocksEditingFactory::allSupportedTypes()
{
	return types;
}

bool BlocksEditingFactory::hasEditor(const QUuid &type)
{
	return editors.contains(type);
}
