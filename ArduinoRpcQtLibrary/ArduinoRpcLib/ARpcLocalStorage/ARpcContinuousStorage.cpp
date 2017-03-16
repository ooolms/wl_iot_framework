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

bool ARpcContinuousStorage::create(const QString &path,const QVector<quint32> &blockNotesSizes)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir(path);
	if(!dir.exists())return false;
	return db->create(dir.absolutePath()+"/data.db",blockNotesSizes);
}

bool ARpcContinuousStorage::openInternal(const QString &path)
{
	return db->open(path+"/data.db");
}

//bool ARpcContinuousStorage::createInternal(const QString &path)
//{
//	QDir dir(path);
//	if(!dir.exists())return false;
//	return db->create(dir.absolutePath()+"/data.db");
//}
