#ifndef SOURCEBLOCK_H
#define SOURCEBLOCK_H

#include "GIDL/core/BaseBlock.h"

class SourceBlock
	:public BaseBlock
{
public:
	explicit SourceBlock(quint32 id);
	virtual ~SourceBlock();
	virtual bool prepareData()=0;

protected:
	virtual void eval()override;

private:
	DataUnit u;
};

#endif // SOURCEBLOCK_H
