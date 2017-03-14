#include "ARpcContinuousStorage.h"
#include <QDir>

ARpcContinuousStorage::ARpcContinuousStorage(QObject *parent)
	:ARpcISensorStorage(parent)
{
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}

bool ARpcContinuousStorage::createInternal(const QString &path)
{
	QDir dir(path);
	if(!dir.exists())return false;
	return db->create(dir.absolutePath()+"/data.db");
}
