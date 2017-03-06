#include "ARpcSensorStorage.h"
#include <QDir>

ARpcSensorStorage::ARpcSensorStorage(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcSensorStorage::create(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return false;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	dir.mkpath(dir.absolutePath()+"/Storage");
	//IMPL
	return true;
}

bool ARpcSensorStorage::isOpened()const
{
	return opened;
}

ARpcSensorStorage::StoreMode ARpcSensorStorage::getStoreMode()const
{
	return storeMode;
}
