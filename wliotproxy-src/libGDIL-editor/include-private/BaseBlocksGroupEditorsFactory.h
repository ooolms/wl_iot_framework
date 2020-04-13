#ifndef BASEBLOCKSGROUPEDITORSFACTORY_H
#define BASEBLOCKSGROUPEDITORSFACTORY_H

#include "GDIL/editor/IBlocksGroupEditorsFactory.h"

class BaseBlocksGroupEditorsFactory
	:public IBlocksGroupEditorsFactory
{
public:
	~BaseBlocksGroupEditorsFactory();
	virtual QStringList allBlocks()const override;
	virtual IBlockEditor *editor(const QString &blockName)override;

protected:
	void addEditor(const QString &blockName,IBlockEditor *editor);

private:
	QStringList mBlockNames;
	QMap<QString,IBlockEditor*> mEditors;
};

#endif // BASEBLOCKSGROUPEDITORSFACTORY_H
