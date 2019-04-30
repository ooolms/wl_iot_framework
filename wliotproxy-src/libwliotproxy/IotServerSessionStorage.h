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

#ifndef IOTSERVERSESSIONSTORAGE_H
#define IOTSERVERSESSIONSTORAGE_H

#include "wliot/storages/ISessionSensorStorage.h"
#include "IotServerStorage.h"

class IotServerSessionStorage
	:public ISessionSensorStorage
{
	Q_OBJECT
public:
	explicit IotServerSessionStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
		const QByteArray &devName,const SensorDef &sensor,ISensorStorage::StoreMode storeMode,
		ISensorStorage::TimestampRule tsRule,SensorDef::Type storedType,QObject *parent=nullptr);
	virtual bool open() override;
	virtual void close() override;
	virtual bool isOpened() const override;
	virtual quint64 valuesCount()override;
	virtual SensorValue *valueAt(quint64 index)override;
	virtual bool listSessions(QList<QUuid> &ids,QByteArrayList &titles)override;
	virtual quint64 valuesCount(const QUuid &sessionId)override;
	virtual SensorValue *valueAt(const QUuid &sessionId,quint64 index)override;
	virtual bool setMainReadSessionId(const QUuid &id)override;
	virtual bool setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)override;
	virtual bool removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)override;
	virtual bool getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)override;
	virtual bool listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)override;
	virtual bool removeSession(const QUuid &sessionId)override;
	virtual bool writeSensorValue(const SensorValue *val)override;
	virtual void writeAttribute(const QByteArray &str, const QByteArray &var) override;
	virtual QByteArray readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QUuid &serviceId)override;
	virtual DataExportConfig getDataExportConfig(const QUuid &serviceId)override;
	virtual void removeDataExportConfig(const QUuid &serviceId)override;
	virtual QList<QUuid> allDataExportServices()override;
	virtual bool createSession(const QByteArray &title, QUuid &sessionId)override;
	virtual bool openSession(const QUuid &sessionId)override;
	virtual bool closeSession(const QUuid &sessionId)override;
	virtual bool isSessionOpened(const QUuid &sessionId)const override;
	virtual bool openMainWriteSession(const QUuid &sessionId)override;
	virtual bool isMainWriteSessionOpened()const override;
	virtual bool closeMainWriteSession()override;
	virtual QUuid getMainWriteSessionId()const override;

	bool values(quint64 startIndex,quint64 count,quint64 step,
		VeryBigArray<SensorValue*> &vals)override;
	bool values(const QUuid &sessionId,quint64 startIndex,
		quint64 count,quint64 step,VeryBigArray<SensorValue*> &vals)override;
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
