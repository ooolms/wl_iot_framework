#ifndef ARPCLASTNVALUESSTORAGE_H
#define ARPCLASTNVALUESSTORAGE_H

#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"

/**
 * @brief The ARpcLastNValuesStorage class
 * Хранит последние N значений в N файлах по принципу циклического FIFO буфера.
 * В файле db.index - индекс, с которого начинается буфер (текстом).
 * Элементы выдаются в порядке записи. Т.е. по индексу 0 база выдает самый старый элемент.
 */
class ARpcLastNValuesStorage
	:public ARpcISensorStorage
{
	Q_OBJECT
public:
	explicit ARpcLastNValuesStorage(ARpcSensor::Type valType,QObject *parent=0);
	bool create(quint32 storedValuesCount,ARpcISensorStorage::TimestampRule rule);

public:
	virtual StoreMode getStoreMode()const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	ARpcISensorValue* readValue(quint32 index);
	quint32 valuesCount();

protected:
	virtual bool openInternal()override;//use dbDir when opening
	virtual void closeInternal()override;
	virtual ARpcSensor::Type effectiveValuesType()const override;

private:
	ARpcDBDriverHelpers hlp;
	quint32 storedCount;
	quint32 startIndex;//индекс первого элемента в циклическом буфере
	bool opened;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCLASTNVALUESSTORAGE_H
