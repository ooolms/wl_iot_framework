#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcContinuousStorage(ARpcSensor::Type valType,QObject *parent=0);
	virtual ~ARpcContinuousStorage();
	virtual StoreMode getStoreMode()const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	virtual ARpcSensor::Type effectiveValuesType()const override;
	bool createAsFixedBlocksDb(const ARpcISensorValue &templateValue,ARpcISensorStorage::TimestampRule rule);
		//TODO переделать поприличнее!!
	bool createAsChainedBlocksDb(ARpcISensorStorage::TimestampRule rule);
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	qint64 valuesCount();
	ARpcISensorValue* valueAt(quint64 index);

protected:
	virtual bool openInternal()override;
	virtual void closeInternal()override;

private:
	QVector<quint32> sizesFromTemplateValue(const ARpcISensorValue &value);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	ARpcDBDriverHelpers hlp;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	bool opened;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCCONTINUOUSSTORAGE_H
