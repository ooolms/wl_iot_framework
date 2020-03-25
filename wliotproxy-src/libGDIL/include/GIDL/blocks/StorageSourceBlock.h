#ifndef STORAGESOURCEBLOCK_H
#define STORAGESOURCEBLOCK_H

#include "GIDL/core/SourceBlock.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"
#include <QPointer>

class StorageSourceBlock
	:public SourceBlock
{
public:
	explicit StorageSourceBlock(ISensorStorage *stor,StorageId stId,quint32 bId,quint32 cnt);
	quint32 count()const;
	void setCount(quint32 c);

protected:
	virtual DataUnit extractDataInternal()override;

private:
	QPointer<ISensorStorage> mStor;
	StorageId mStorId;
	quint32 mCount;
};

#endif // STORAGESOURCEBLOCK_H
