/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef IOTSERVERSTORAGESCOMMANDS_H
#define IOTSERVERSTORAGESCOMMANDS_H

#include <QObject>
#include "IotServerConnection.h"
#include "wliot/storages/VeryBigArray.h"
#include "IotServerTypes.h"

class IotServerStoragesCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerStoragesCommands(IotServerConnection *conn);
	bool listStorages(QList<IotServerStorageDescr> &stors);
	bool addStorage(const QByteArray &devIdOrName,const QByteArray &sensorName,ISensorStorage::StoreMode storeMode,
		ISensorStorage::TimestampRule tsRule,int valuesCount=1);
	bool removeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
	bool storageAddDataExport(const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &serviceId,
		const QMap<QByteArray,QByteArray> &serviceConfig);
	bool storageGetDataExport(const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &serviceId,
		QMap<QByteArray,QByteArray> &serviceConfig);
	bool storageAllDataExports(const QByteArray &devIdOrName,const QByteArray &sensorName,QList<QUuid> &services);
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
		quint64 step,const SensorDef::Type &sensorType,VeryBigArray<SensorValue*> &values,
		const QUuid &sessionId=QUuid());
	bool setStorageAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,
		const QByteArray &attrValue);
	bool getStorageAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,
		QByteArray &attrValue);
	bool availableDataExportServices(QList<IotServerDataExportServiceDescr> &services);
	bool alterozoomAuth(const QByteArray &host,const QByteArray &email,const QByteArray &pass);
	bool alterozoomList(QList<QPair<QByteArray,QByteArray>> &hostEmailList);

public:
	static bool storageFromArgs(const QByteArrayList &args,IotServerStorageDescr &st);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERSTORAGESCOMMANDS_H
