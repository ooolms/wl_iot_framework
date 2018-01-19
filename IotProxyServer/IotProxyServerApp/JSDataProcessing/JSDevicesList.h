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

#ifndef JSDEVICESLIST_H
#define JSDEVICESLIST_H

#include <QObject>
#include <QScriptEngine>
#include <QUuid>

class JSDevicesList
	:public QObject
{
	Q_OBJECT

public:
	explicit JSDevicesList(QScriptEngine *e,QObject *parent=nullptr);

	/**
	 * @brief devices
	 * @return список (array) идентификаторов устройств
	 */
	Q_INVOKABLE QScriptValue devices();
	Q_INVOKABLE QScriptValue device(QScriptValue idOrNameStr);
	Q_INVOKABLE QScriptValue registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr);
	Q_INVOKABLE QScriptValue registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,QScriptValue sensorsXml);
	Q_INVOKABLE QScriptValue registerVirtualDevice(QScriptValue idStr,QScriptValue nameStr,
		QScriptValue sensorsXml,QScriptValue controlsXml);//CRIT strings -> objects

signals:
	void deviceIdentified(QScriptValue id,QScriptValue name);
	void deviceDisconnected(QScriptValue id);

private slots:
	void onDeviceIdentified(QUuid id,QString name);
	void onDeviceDisconnected(QUuid id);

private:
	QScriptEngine *js;
};

#endif // JSDEVICESLIST_H
