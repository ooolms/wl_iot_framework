#include "BaseBlocksGroupEditorsFactory.h"

BaseBlocksGroupEditorsFactory::~BaseBlocksGroupEditorsFactory()
{
	for(auto v:mEditors)
		delete v;
}

QStringList BaseBlocksGroupEditorsFactory::allBlocks()const
{
	return mBlockNames;
}

IBlockEditor *BaseBlocksGroupEditorsFactory::editor(const QString &blockName)
{
	return mEditors.value(blockName);
}

void BaseBlocksGroupEditorsFactory::addEditor(const QString &blockName, IBlockEditor *editor)
{
	mBlockNames.append(blockName);
	mEditors[blockName]=editor;
}
