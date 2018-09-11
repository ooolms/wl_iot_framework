#ifndef IOTSERVERSESSIONSTORAGE_H
#define IOTSERVERSESSIONSTORAGE_H

#include "ARpcStorages/ARpcISessionSensorStorage.h"
#include "IotServerStorage.h"

class IotServerSessionStorage
	:public ARpcISessionSensorStorage
{
	Q_OBJECT
public:
	explicit IotServerSessionStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
		const QByteArray &devName,const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
		ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent=nullptr);
	virtual bool open() override;
	virtual void close() override;
	virtual bool isOpened() const override;
	virtual quint64 valuesCount()override;
	virtual ARpcSensorValue *valueAt(quint64 index)override;
	virtual bool listSessions(QList<QUuid> &ids,QByteArrayList &titles)override;
	virtual quint64 valuesCount(const QUuid &sessionId)override;
	virtual ARpcSensorValue *valueAt(const QUuid &sessionId,quint64 index)override;
	virtual bool setMainReadSessionId(const QUuid &id)override;
	virtual bool setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)override;
	virtual bool removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)override;
	virtual bool getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)override;
	virtual bool listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)override;
	virtual bool removeSession(const QUuid &sessionId)override;
	virtual bool writeSensorValue(const ARpcSensorValue *val) override;
	virtual void writeAttribute(const QByteArray &str, const QVariant &var) override;
	virtual QVariant readAttribute(const QByteArray &str) override;
	virtual void addDataExportConfig(const QByteArray &serviceType, const DataExportConfig &cfg) override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType) override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType) override;
	virtual void removeDataExportConfig(const QByteArray &serviceType) override;
	virtual QByteArrayList allDataExportServices() override;
	virtual bool createSession(const QByteArray &title, QUuid &sessionId) override;
	virtual bool openSession(const QUuid &sessionId) override;
	virtual bool closeSession(const QUuid &sessionId) override;
	virtual bool isSessionOpened(const QUuid &sessionId) const override;
	virtual bool openMainWriteSession(const QUuid &sessionId) override;
	virtual bool isMainWriteSessionOpened() const override;
	virtual bool closeMainWriteSession() override;
	virtual QUuid getMainWriteSessionId() const override;

	bool values(quint64 startIndex,quint64 count,quint64 step,
		VeryBigArray<ARpcSensorValue*> &vals)override;
	bool values(const QUuid &sessionId,quint64 startIndex,
		quint64 count,quint64 step,VeryBigArray<ARpcSensorValue*> &vals)override;
	void setClosedWhenSrvDisconnected();

protected:
	void onNewValueFromServer(const QByteArrayList &vArgs);

private:
	friend class IotServerStoragesDatabase;
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	bool mIsOpened;
	QUuid mainReadId;
};

#endif // IOTSERVERSESSIONSTORAGE_H
