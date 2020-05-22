#ifndef IDEVICESNAMESDATABASE_H
#define IDEVICESNAMESDATABASE_H

#include <QByteArrayList>
#include <QUuid>
#include <QObject>

namespace WLIOT
{
	class IDevicesNamesDatabase
		:public QObject
	{
	public:
		explicit IDevicesNamesDatabase(QObject *parent=0):QObject(parent){}
		virtual ~IDevicesNamesDatabase(){}
		virtual QByteArray deviceName(const QUuid &uid)=0;
		virtual QByteArrayList devicesNames(const QList<QUuid> &uids)=0;
		virtual QUuid findDevice(const QByteArray &name)=0;
		virtual bool setManualDevName(const QUuid &uid,const QByteArray &name)=0;
	};
}

#endif // IDEVICESNAMESDATABASE_H
