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

#ifndef JSISENSORSTORAGE_H
#define JSISENSORSTORAGE_H

#include <QObject>
#include "wliot/storages/BaseFSSensorStorage.h"
#include <QScriptEngine>

class JSISensorStorage
	:public QObject
{
	Q_OBJECT

public:
	explicit JSISensorStorage(QScriptEngine *e,ISensorStorage *st,QObject *parent=nullptr);
	Q_INVOKABLE bool isOpened();
	Q_INVOKABLE QString valuesType();
	Q_INVOKABLE QString deviceId();
	Q_INVOKABLE QString deviceName();
	Q_INVOKABLE QString sensorName();
	Q_INVOKABLE QVariant readAttribute(QString str);
	Q_INVOKABLE quint64 valuesCount();
	Q_INVOKABLE QScriptValue valueAt(quint64 index);
	Q_INVOKABLE QString getStoreMode();

signals:
	void newValueWritten(QScriptValue value);

private slots:
	void onNewValueDirect(const SensorValue *value);
	void onNewValueQueued(void *value);

protected:
	ISensorStorage *stor;
	QScriptEngine *js;
};

#endif // JSISENSORSTORAGE_H
