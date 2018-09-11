#ifndef ARPCFSSENSORSTORAGEHELPER_H
#define ARPCFSSENSORSTORAGEHELPER_H

#include <QObject>
#include <QSettings>
#include "ARpcStorages/ARpcISensorStorage.h"

class ARpcFSSensorStorageHelper
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcFSSensorStorageHelper(const QString &path,QObject *parent=nullptr);
	virtual ~ARpcFSSensorStorageHelper();
//	void setDBPath(const QString &path);
	QString dbPath();
	void writeAttribute(const QByteArray &str,const QVariant &var);
	QVariant readAttribute(const QByteArray &str);
	void addDataExportConfig(const QByteArray &serviceType,const ARpcISensorStorage::DataExportConfig &cfg);
	bool hasDataExportConfig(const QByteArray &serviceType);
	ARpcISensorStorage::DataExportConfig getDataExportConfig(const QByteArray &serviceType);
	void removeDataExportConfig(const QByteArray &serviceType);
	QByteArrayList allDataExportServices();
	void storeDeviceName(const QByteArray &name);
	QSettings* settings();

	static ARpcISensorStorage* preCreate(const QString &path,const QUuid &devId,
		const QByteArray &devName,ARpcSensorDef sensor,ARpcISensorStorage::StoreMode mStoreMode,
		ARpcISensorStorage::TimestampRule rule);
	//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ARpcISensorStorage* preOpen(const QString &path);

private:
	static ARpcISensorStorage* makeStorage(const QString &path,
		const QUuid &devId,const QByteArray &devName,const ARpcSensorDef &sensor,
		ARpcISensorStorage::StoreMode mode,ARpcISensorStorage::TimestampRule rule);

private:
	QString mDbPath;
	QSettings *mSettings,*mExportSettings;
};

#endif // ARPCFSSENSORSTORAGEHELPER_H
