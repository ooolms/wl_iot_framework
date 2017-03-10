#include "ARpcISensorStorage.h"
#include <QDir>

ARpcISensorStorage::ARpcISensorStorage(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcISensorStorage::create(const QString &path, StoreMode mode)
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

bool ARpcISensorStorage::isOpened()const
{
	return opened;
}

ARpcISensorStorage::StoreMode ARpcISensorStorage::getStoreMode()const
{
	return storeMode;
}

ARpcISensorStorage *ARpcISensorStorage::create(const QString &path,ARpcISensorStorage::StoreMode mode)
{
	//IMPL
}

ARpcISensorStorage *ARpcISensorStorage::open(const QString &path)
{
	//IMPL
}
