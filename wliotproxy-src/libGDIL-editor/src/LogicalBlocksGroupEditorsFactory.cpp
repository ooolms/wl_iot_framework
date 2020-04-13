#include "LogicalBlocksGroupEditorsFactory.h"
#include "GDIL/blocks/LogicalBlocks.h"
#include "block_editors/DefaultBlockEditor.h"

LogicalBlocksGroupEditorsFactory::LogicalBlocksGroupEditorsFactory()
{
	addEditor(NotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_not.png"),"logical NOT","NOT"));
	addEditor(AndBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_and.png"),"logical AND","AND"));
	addEditor(OrBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_or.png"),"logical OR","OR"));
	addEditor(XorBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_xor.png"),"logical XOR","XOR"));
	addEditor(AndNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_and_not.png"),"logical AND-NOT","AND-NOT"));
	addEditor(OrNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_or_not.png"),"logical OR-NOT","OR-NOT"));
	addEditor(XorNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/GDIL/editor/blocks/logic_xor_not.png"),"logical XOR-NOT","XOR-NOT"));
}

QString LogicalBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	return "Logical blocks";
}
