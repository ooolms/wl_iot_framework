#include "ARpcDBDriverGTimeIndex.h"

static const QVector<quint32> sizes=QVector<quint32>()<<sizeof(qint64)<<sizeof(quint64);

ARpcDBDriverGTimeIndex::ARpcDBDriverGTimeIndex(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcDBDriverGTimeIndex::create(const QString &filePath)
{
	return dbDriver->create(filePath,sizes);
}

bool ARpcDBDriverGTimeIndex::open(const QString &filePath)
{
	if(!dbDriver->open(filePath))return false;
	loadIndex();
	return true;
}

bool ARpcDBDriverGTimeIndex::isOpened()
{
	return dbDriver->isOpened();
}
