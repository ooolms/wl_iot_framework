#ifndef REMOTESTORAGE_H
#define REMOTESTORAGE_H

#include "ARpcStorages/ARpcISensorStorage.h"
#include "IotServerCommands.h"

class RemoteStorage
	:public ARpcISensorStorage
{
	Q_OBJECT
public:
	explicit RemoteStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
		const QByteArray &devName,const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
		ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent=nullptr);
	virtual ~RemoteStorage();
	virtual quint64 valuesCount()override;
	virtual bool isOpened()const override;
	virtual bool open()override;
	virtual void close()override;
	virtual ARpcSensorValue* valueAt(quint64 index)override;
	virtual bool writeSensorValue(const ARpcSensorValue *val)override;
	virtual void writeAttribute(const QByteArray &str,const QVariant &var)override;
	virtual QVariant readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)override;
	virtual void removeDataExportConfig(const QByteArray &serviceType)override;
	virtual QByteArrayList allDataExportServices()override;
	virtual QList<ARpcSensorValue*> values(quint64 index,quint64 count,quint64 step);
	void setClosedWhenSrvDisconnected();

protected:
	void onNewValueFromServer(const QByteArrayList &vArgs);

protected:
	friend class RemoteStorages;
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	bool mIsOpened;
};

#endif // REMOTESTORAGE_H
