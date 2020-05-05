#ifndef FSDEVICESNAMESDATABASE_H
#define FSDEVICESNAMESDATABASE_H

#include "wliot/IDevicesNamesDatabase.h"
#include <QMap>

class FSDevicesNamesDatabase
	:public IDevicesNamesDatabase
{
public:
	explicit FSDevicesNamesDatabase(QObject *parent=0);
	virtual ~FSDevicesNamesDatabase();
	bool initDb(const QString &path);
	virtual QByteArray deviceName(const QUuid &uid)override;
	virtual QByteArrayList devicesNames(const QList<QUuid> &uids)override;
	virtual QUuid findDevice(const QByteArray &name)override;
	virtual bool setManualDevName(const QUuid &uid,const QByteArray &name)override;

public slots:
	void onDeviceIdentified(const QUuid &uid,const QByteArray &name);
	void onNameFromStorage(const QUuid &uid,const QByteArray &name);

private:
	bool writeCfg();

private:
	QMap<QUuid,QByteArray> autoDevNames,manualDevNames;
	QMap<QByteArray,QUuid> reverseMap;
	QString dbPath;
};

#endif // FSDEVICESNAMESDATABASE_H
