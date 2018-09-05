#ifndef IOTSERVERSTORAGESCOMMANDS_H
#define IOTSERVERSTORAGESCOMMANDS_H

#include <QObject>
#include "IotServerConnection.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "IotServerTypes.h"

class IotServerStoragesCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerStoragesCommands(IotServerConnection *conn,QObject *parent=nullptr);
	bool listStorages(QList<IotServerStorageDescr> &stors);
	bool addStorage(const QByteArray &devIdOrName,const QByteArray &sensorName,ARpcISensorStorage::StoreMode storeMode,
		ARpcISensorStorage::TimestampRule tsRule,int valuesCount=1);
	bool removeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
	bool bindStorage(const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &serviceName,
		const QMap<QByteArray,QByteArray> &serviceConfig);
	bool listSessions(const QByteArray &devIdOrName,const QByteArray &sensorName,
		QList<QUuid> &ids,QByteArrayList &titles);
	bool listSessionAttrs(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QUuid &sessionId,QMap<QByteArray,QByteArray> &attrs);
	bool getSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QUuid &sessionId,const QByteArray &attrName,QByteArray &value);
	bool setSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QUuid &sessionId,const QByteArray &attrName,const QByteArray &value);
	bool removeSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QUuid &sessionId,const QByteArray &attrName);
	bool startSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QUuid &sessionId);
	bool createSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
		const QByteArray &sessionTitle,QUuid &sessionId);
	bool stopSession(const QByteArray &devIdOrName,const QByteArray &sensorName);
	bool removeSession(const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId);
	bool continueSession(const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId);
	bool getSessionWriteId(const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &sessionId);
	bool getSamplesCount(const QByteArray &devIdOrName,const QByteArray &sensorName,quint64 &count,
		const QUuid &sessionId=QUuid());
	bool getSamples(const QByteArray &devIdOrName,const QByteArray &sensorName,quint64 startIndex,quint64 count,
		quint64 step,const ARpcSensorDef::Type &sensorType,QList<ARpcSensorValue*> &values,
		const QUuid &sessionId=QUuid());
	bool registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,const QByteArray &sensorsDef);
	bool sendVDevMeasurement(const QUuid &deviceId,const QByteArray &sensorName,const QByteArrayList &measArgs);

public:
	static bool storageFromArgs(const QByteArrayList &args,IotServerStorageDescr &st);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERSTORAGESCOMMANDS_H
