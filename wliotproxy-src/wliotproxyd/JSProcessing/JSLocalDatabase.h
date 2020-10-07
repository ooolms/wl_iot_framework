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

#ifndef JSLOCALDATABASE_H
#define JSLOCALDATABASE_H

#include "wliot/storages/FSStoragesDatabase.h"
#include "JSISensorStorage.h"
#include <QScriptEngine>
#include "../AccessManagement/AccessPolicyTypes.h"

class JSStoragesDatabase
	:public QObject
{
	Q_OBJECT

public:
	explicit JSStoragesDatabase(QScriptEngine *e,WLIOT::FSStoragesDatabase *db,IdType uid);
	Q_INVOKABLE bool isOpened();
	Q_INVOKABLE QScriptValue listSensors();

	/**
	 * @brief existingStorage
	 * @param obj объект, идентифицирующий хранилище
	 *	"deviceId" -> идентификатор устройства
	 *	"sensorName" -> имя датчика
	 * @return
	 */
	Q_INVOKABLE QScriptValue existingStorage(QScriptValue obj);

	/**
	 * @brief existingStorage
	 * @param obj объект, идентифицирующий хранилище
	 *	"deviceId" -> идентификатор устройства
	 *	"sensorName" -> имя датчика
	 *	"storeMode" -> тип хранения данных ("continuous", "manual_sessions" или "last_n_values")
	 *	"tsRule" -> правило преобразования временной метки ("dont_touch", "add_global_time" или "drop_time"),
	 *		по-умолчанию add_global_time
	 *	"N" - N для хранилища типа last_n_values
	 * @return
	 */
	Q_INVOKABLE QScriptValue createStorage(QScriptValue obj);

signals:
	void opened();
	void closed();
	void storageCreated(QScriptValue obj);
	void storageRemoved(QScriptValue obj);

private slots:
	void onOpened();
	void onClosed();
	void onStorageCreated(const WLIOT::StorageId &id);
	void onStorageRemoved(const WLIOT::StorageId &id);

private:
	JSISensorStorage* makeJSStorage(WLIOT::ISensorStorage *st);

private:
	WLIOT::FSStoragesDatabase *dBase;
	QScriptEngine *js;
	QMap<WLIOT::StorageId,JSISensorStorage*> storages;
	IdType ownerUid;
};

#endif // JSLOCALDATABASE_H
