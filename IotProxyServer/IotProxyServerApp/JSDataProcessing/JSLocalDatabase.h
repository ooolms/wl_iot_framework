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

#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include "JSISensorStorage.h"
#include <QScriptEngine>

class JSLocalDatabase
	:public QObject
{
	Q_OBJECT

public:
	explicit JSLocalDatabase(QScriptEngine *e,ARpcLocalDatabase *db,QObject *parent=0);
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
	void onStorageCreated(const DeviceAndSensorId &id);
	void onStorageRemoved(const DeviceAndSensorId &id);

private:
	JSISensorStorage* makeJSStorage(ARpcISensorStorage *st);

private:
	ARpcLocalDatabase *dBase;
	QScriptEngine *js;
	QList<DeviceAndSensorId> storagesIds;
	QList<JSISensorStorage*> storages;
	//TODO replace 2 QList with 1 QMap ?
};

#endif // JSLOCALDATABASE_H
