#ifndef STORAGESOURCEBLOCK_H
#define STORAGESOURCEBLOCK_H

#include "GDIL/core/SourceBlock.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"
#include <QPointer>

class StorageSourceBlock
	:public SourceBlock
{
public:
	explicit StorageSourceBlock(quint32 bId=0);
	void setParams(StorageId stId,const QString &devName,SensorDef::Type valType,quint32 cnt);
	virtual QUuid typeId()const override;
	quint32 count()const;
	const StorageId& storageId()const;
	const QString& devName()const;
	const SensorDef::Type valuesType()const;

protected:
	virtual DataUnit extractDataInternal()override;

public:
	static const QUuid mTypeId;

private:
	StorageId mStorId;
	QString mDevName;
	quint32 mCount;
	SensorDef::Type mValType;
};

#endif // STORAGESOURCEBLOCK_H
