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

#ifndef STORAGESDATABASE_H
#define STORAGESDATABASE_H

#include "wliot/client/ServerDataTypes.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/client/AllServerCommands.h"
#include "wliot/storages/IStoragesDatabase.h"
#include <QByteArrayList>

namespace WLIOTClient
{
	/**
	* @brief Высокоуровневый класс для доступа к хранилищам на сервере
	*/
	class StoragesDatabase
		:public WLIOT::IStoragesDatabase
	{
		Q_OBJECT
	public:
		explicit StoragesDatabase(ServerConnection *conn,AllServerCommands *cmds);
		virtual bool listStorages(QList<WLIOT::StorageId> &list)override;
		virtual bool listStoragesWithDevNames(QList<WLIOT::StorageId> &list,QByteArrayList &titles)override;
		virtual WLIOT::ISensorStorage *existingStorage(const WLIOT::StorageId &id)override;
		virtual WLIOT::ISensorStorage* create(const QUuid &devId,const QByteArray &devName,
			WLIOT::ISensorStorage::StoreMode mode,const WLIOT::SensorDef &sensor,
			WLIOT::ISensorStorage::TimestampRule rule,int valuesCount=1,bool gtIndex=false)override;
		virtual bool hasStorage(const WLIOT::StorageId &id)override;
		virtual bool removeStorage(const WLIOT::StorageId &id)override;
		virtual QUuid findDeviceId(const QByteArray &devIdOrName)override;
		virtual WLIOT::ISensorStorage *findStorageForDevice(
			const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)override;
		virtual QByteArray findDevName(const QUuid &devId)override;

	private slots:
		void onStorageCreatedFromServer(const StorageDescr &s);
		void onStorageRemovedFromServer(const WLIOT::StorageId &id);
		void onConnected();
		void onDisconnected();
		void onNewValue(const WLIOT::StorageId &id,const QByteArrayList &args);

	private:
		WLIOT::ISensorStorage* createWrap(const StorageDescr &s);

	private:
		ServerConnection *srvConn;
		AllServerCommands *commands;
		QMap<WLIOT::StorageId,WLIOT::ISensorStorage*> storages;
	};
}

#endif // STORAGESDATABASE_H
