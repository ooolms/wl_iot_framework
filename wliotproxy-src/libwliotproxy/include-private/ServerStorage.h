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

#ifndef SERVERSTORAGE_H
#define SERVERSTORAGE_H

#include "wliot/storages/ISensorStorage.h"
#include "wliot/client/AllServerCommands.h"
#include "wliot/storages/VeryBigArray.h"

namespace WLIOTClient
{
	class ServerStorage
		:public WLIOT::ISensorStorage
	{
		Q_OBJECT
	public:
		explicit ServerStorage(ServerConnection *conn,AllServerCommands *cmds,const QUuid &devId,
			const QByteArray &devName,const WLIOT::SensorDef &sensor,ISensorStorage::StoreMode storeMode,
			ISensorStorage::TimestampRule tsRule,WLIOT::SensorDef::Type storedType,QObject *parent=nullptr);
		virtual ~ServerStorage();
		virtual quint64 valuesCount()override;
		virtual bool isOpened()const override;
		virtual bool open()override;
		virtual void close()override;
		virtual WLIOT::SensorValue* valueAt(quint64 index)override;
		virtual bool writeSensorValue(const WLIOT::SensorValue *val)override;
		virtual void writeAttribute(const QByteArray &str,const QByteArray &var)override;
		virtual QByteArray readAttribute(const QByteArray &str)override;
		virtual void addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)override;
		virtual bool hasDataExportConfig(const QUuid &serviceId)override;
		virtual DataExportConfig getDataExportConfig(const QUuid &serviceId)override;
		virtual void removeDataExportConfig(const QUuid &serviceId)override;
		virtual QList<QUuid> allDataExportServices()override;
		virtual bool values(quint64 index,quint64 count,quint64 step,
			WLIOT::VeryBigArray<WLIOT::SensorValue*> &vals)override;
		void setClosedWhenSrvDisconnected();
		virtual WLIOT::SensorValue* lastValue()override;

	protected:
		void onNewValueFromServer(const QByteArrayList &vArgs);

	private:
		friend class StoragesDatabase;
		AllServerCommands *commands;
		ServerConnection *srvConn;
		bool mIsOpened,mHasValuesCount;
		quint64 mValuesCount;
		QScopedPointer<WLIOT::SensorValue> mLastValue;
	};
}

#endif // SERVERSTORAGE_H
