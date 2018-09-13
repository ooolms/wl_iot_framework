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

#ifndef ARPCISESSIONSENSORSTORAGE_H
#define ARPCISESSIONSENSORSTORAGE_H

#include <QObject>
#include <QByteArrayList>
#include <QUuid>
#include "ARpcStorages/ARpcISensorStorage.h"
#include "ARpcBase/ARpcSensorValue.h"

class ARpcISessionSensorStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcISessionSensorStorage(const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,
		StoreMode stMode,ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedValType,
		QObject *parent=nullptr);
	virtual ~ARpcISessionSensorStorage(){}
	virtual bool listSessions(QList<QUuid> &ids,QByteArrayList &titles)=0;
	virtual bool createSession(const QByteArray &title,QUuid &sessionId)=0;
	virtual bool openSession(const QUuid &sessionId)=0;
	virtual bool closeSession(const QUuid &sessionId)=0;
	virtual bool removeSession(const QUuid &sessionId)=0;
	virtual bool setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)=0;
	virtual bool removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)=0;
	virtual bool getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)=0;
	virtual bool listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)=0;
	virtual quint64 valuesCount(const QUuid &sessionId)=0;
	using ARpcISensorStorage::valuesCount;
	virtual ARpcSensorValue* valueAt(const QUuid &sessionId,quint64 index)=0;
	using ARpcISensorStorage::valueAt;
	virtual bool isSessionOpened(const QUuid &sessionId)const=0;
	virtual bool openMainWriteSession(const QUuid &sessionId)=0;
	virtual bool isMainWriteSessionOpened()const=0;
	virtual bool closeMainWriteSession()=0;
	virtual QUuid getMainWriteSessionId()const=0;
	virtual bool setMainReadSessionId(const QUuid &id)=0;
	virtual bool values(const QUuid &sessionId,quint64 index,quint64 count,quint64 step,
		VeryBigArray<ARpcSensorValue*> &vals);
	using ARpcISensorStorage::values;
};

#endif // ARPCISESSIONSENSORSTORAGE_H
