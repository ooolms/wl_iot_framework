#include "CoreBlocksGroupEditorsFactory.h"
#include "block_editors/CommandBlockEditor.h"
#include "block_editors/ComparationBlockEditor.h"
#include "block_editors/DebugBlockEditor.h"
#include "block_editors/DefaultBlockEditor.h"
#include "block_editors/DimChangeBlockEditor.h"
#include "block_editors/NormingBlockEditor.h"
#include "block_editors/StaticSourceBlockEditor.h"
#include "block_editors/StorageSourceBlockEditor.h"
#include "GDIL/blocks/AllBlocks.h"

CoreBlocksGroupEditorsFactory::CoreBlocksGroupEditorsFactory()
{
	addEditor(CommandBlock::mBlockName,new CommandBlockEditor);
	addEditor(StaticSourceBlock::mBlockName,new StaticSourceBlockEditor);
	addEditor(StorageSourceBlock::mBlockName,new StorageSourceBlockEditor);
	addEditor(AverageCalcBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/average_calc.png"),"average calculation","average calc"));
	addEditor(ComparationBlock::mBlockName,new ComparationBlockEditor);
	addEditor(DimChangeBlock::mBlockName,new DimChangeBlockEditor);
	addEditor(ConditionTransactionBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/condition_transaction.png"),"condition transaction","condition transaction"));
	addEditor(NormingBlock::mBlockName,new NormingBlockEditor);
	addEditor(DebugBlock::mBlockName,new DebugBlockEditor);
}

QString CoreBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	 return "Core blocks";
}
