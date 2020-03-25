#ifndef STATICSOURCEBLOCK_H
#define STATICSOURCEBLOCK_H

#include "GIDL/core/SourceBlock.h"

class StaticSourceBlock
	:public SourceBlock
{
public:
	explicit StaticSourceBlock(const DataUnit &t,quint32 bId);
	const DataUnit& data()const;
	void setData(const DataUnit &u);

protected:
	virtual DataUnit extractDataInternal()override;

private:
	DataUnit mStaticData;
};

#endif // STATICSOURCEBLOCK_H
