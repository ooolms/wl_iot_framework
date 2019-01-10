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

#ifndef IOTSERVERSTORAGE_H
#define IOTSERVERSTORAGE_H

#include "ARpcStorages/ARpcISensorStorage.h"
#include "IotServerCommands.h"
#include "ARpcStorages/VeryBigArray.h"

class IotServerStorage
	:public ARpcISensorStorage
{
	Q_OBJECT
public:
	explicit IotServerStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
		const QByteArray &devName,const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
		ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent=nullptr);
	virtual ~IotServerStorage();
	virtual quint64 valuesCount()override;
	virtual bool isOpened()const override;
	virtual bool open()override;
	virtual void close()override;
	virtual ARpcSensorValue* valueAt(quint64 index)override;
	virtual bool writeSensorValue(const ARpcSensorValue *val)override;
	virtual void writeAttribute(const QByteArray &str,const QByteArray &var)override;
	virtual QByteArray readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)override;
	virtual void removeDataExportConfig(const QByteArray &serviceType)override;
	virtual QByteArrayList allDataExportServices()override;
	virtual bool values(quint64 index,quint64 count,quint64 step,
		VeryBigArray<ARpcSensorValue*> &vals)override;
	void setClosedWhenSrvDisconnected();

protected:
	void onNewValueFromServer(const QByteArrayList &vArgs);

private:
	friend class IotServerStoragesDatabase;
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	bool mIsOpened;
};

#endif // IOTSERVERSTORAGE_H
