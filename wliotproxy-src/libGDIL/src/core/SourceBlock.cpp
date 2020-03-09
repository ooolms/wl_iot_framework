#include "GIDL/core/SourceBlock.h"

SourceBlock::SourceBlock(quint32 id)
	:BaseBlock(id)
	,u(DataUnit::SINGLE)
{
}

void SourceBlock::eval()
{
	for(int i=0;i<outputs.count();++i)
		outputs[i]->setData(u);
}
