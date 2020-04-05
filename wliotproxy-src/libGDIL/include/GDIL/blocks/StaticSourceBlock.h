#ifndef STATICSOURCEBLOCK_H
#define STATICSOURCEBLOCK_H

#include "GDIL/core/SourceBlock.h"

class StaticSourceBlock
	:public SourceBlock
{
public:
	explicit StaticSourceBlock(quint32 bId=0);
	const DataUnit& value()const;
	void setValue(const DataUnit &u);
	virtual QUuid typeId()const override;

protected:
	virtual DataUnit extractDataInternal()override;

public:
	static const QUuid mTypeId;

private:
	DataUnit mValue;
};

#endif // STATICSOURCEBLOCK_H
