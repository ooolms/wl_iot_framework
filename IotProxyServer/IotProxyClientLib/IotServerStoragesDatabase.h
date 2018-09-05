#ifndef REMOTESTORAGES_H
#define REMOTESTORAGES_H

#include "IotServerTypes.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"
#include "ARpcStorages/ARpcIStoragesDatabase.h"
#include "IotServerStorage.h"
#include <QByteArrayList>

class IotServerStoragesDatabase
	:public ARpcIStoragesDatabase
{
	Q_OBJECT
public:
	explicit IotServerStoragesDatabase(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=0);
	bool listStorages(QList<ARpcStorageId> &list);
	bool listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles);
	IotServerStorage *existingStorage(const ARpcStorageId &id);
	IotServerStorage* create(const QUuid &devId,const QByteArray &devName,ARpcISensorStorage::StoreMode mode,
		const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,
		int valuesCount);
	bool hasStorage(const ARpcStorageId &id);
	bool removeStorage(const ARpcStorageId &id);

signals:
	void storageCreated(const ARpcStorageId &id);
	void storageRemoved(const ARpcStorageId &id);

private slots:
	void onStorageCreatedFromServer(const IotServerStorageDescr &s);
	void onStorageRemovedFromServer(const ARpcStorageId &id);
	void onServerConnected();
	void onServerDisconnected();
	void onNewValue(const ARpcStorageId &id,const QByteArrayList &args);

private:
	IotServerStorage* createWrap(const IotServerStorageDescr &s);

private:
	IotServerConnection *srvConn;
	IotServerCommands *commands;
	QMap<ARpcStorageId,IotServerStorage*> storages;
};

#endif // REMOTESTORAGES_H
