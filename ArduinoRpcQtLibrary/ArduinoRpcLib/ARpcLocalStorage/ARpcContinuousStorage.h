#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
#include "ARpcLocalStorage/ARpcDBDriverGTimeIndex.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcContinuousStorage(ARpcSensor::Type valType,QObject *parent=0);
	virtual ~ARpcContinuousStorage();
	bool createAsFixedBlocksDb(const ARpcISensorValue &templateValue,
		ARpcISensorStorage::TimestampRule rule,bool gtIndex=false);
	bool createAsChainedBlocksDb(ARpcISensorStorage::TimestampRule rule,bool gtIndex=false);
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	quint64 valuesCount();
	ARpcISensorValue* valueAt(quint64 index);
	quint64 findInGTIndex(qint64 ts);
	virtual bool isOpened()const override;

public:
	virtual StoreMode getStoreMode()const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	virtual ARpcSensor::Type effectiveValuesType()const override;
	virtual bool open()override;

protected:
	virtual void closeInternal()override;

private:
	QVector<quint32> sizesFromTemplateValue(const ARpcISensorValue &value);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	ARpcDBDriverGTimeIndex *indDb;
	ARpcDBDriverHelpers hlp;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	bool opened;
	bool hasIndex;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCCONTINUOUSSTORAGE_H
