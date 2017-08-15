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

#ifndef ARPCSESSIONSTORAGE_H
#define ARPCSESSIONSTORAGE_H

#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
#include "ARpcLocalStorage/ARpcDBDriverGTimeIndex.h"
#include <QUuid>

/**
 * @brief The ARpcSessionStorage class
 * Сессии хранятся в поддиректории "sessions" основной директории базы.
 * Каждая сессия - папка с названием в виде UID ("{00000000-0000-0000-0000-000000000000}").
 * В папке лежит отдельный файл с названием сессии "title.txt",
 * ini-файл с пользовательскими атрибутами и сама база со значениями.
 */
class ARpcSessionStorage
	:public ARpcISensorStorage
{
private:
	struct Session
	{
		union
		{
			ARpcDBDriverFixedBlocks *fbDb;
			ARpcDBDriverChainedBlocks *cbDb;
		};
		ARpcDBDriverGTimeIndex *indDb;
		QMap<QString,QVariant> attributes;
	};

	Q_OBJECT
public:
	explicit ARpcSessionStorage(bool autoSess,ARpcSensor::Type valType,QObject *parent=0);
	virtual ~ARpcSessionStorage();
	bool createAsFixedBlocksDb(const ARpcISensorValue &templateValue,bool gtIndex=false);
	bool createAsChainedBlocksDb(bool gtIndex=false);
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	bool listSessions(QList<QUuid> &ids,QStringList &titles);
	bool createSession(const QString &title,QUuid &sessionId);
	bool openMainWriteSession(const QUuid &sessionId);
	bool openSession(const QUuid &sessionId);
	bool closeMainWriteSession();
	bool closeSession(const QUuid &sessionId);
	bool removeSession(const QUuid &sessionId);
	bool setSessionAttribute(const QUuid &sessionId,const QString &key,const QVariant &val);
	bool getSessionAttribute(const QUuid &sessionId,const QString &key,QVariant &val);
	quint64 valuesCount(const QUuid &sessionId);
	quint64 findInGTIndex(const QUuid &sessionId,qint64 ts);
	ARpcISensorValue* valueAt(const QUuid &sessionId,quint64 index);
	bool isSessionOpened(const QUuid &sessionId)const;
	bool isMainWriteSessionOpened()const;
	QUuid getMainWriteSessionId()const;
	bool setMainReadSessionId(const QUuid &id);

public:
	virtual bool open()override;
	virtual StoreMode getStoreMode()const;
	virtual ARpcSensor::Type effectiveValuesType()const;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	virtual TimestampRule fixTimestampRule(TimestampRule rule)override;
	virtual bool isOpened()const override;
	virtual quint64 valuesCount()override;
	virtual ARpcISensorValue* valueAt(quint64 index)override;

protected:
	virtual void closeInternal()override;

private:
	QString blockNoteSizesToString();
	bool parseBlockNoteSizes(const QString &str);
	void closeSessionAndDeleteDb(Session &d);

private:
	QMap<QUuid,Session> sessions;
	ARpcDBDriverHelpers hlp;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	QVector<quint32> blockNoteSizesForSessions;
	bool autoSessions;
	bool opened;
	bool hasIndex;
	QUuid mainWriteSessionId;
	Session mainWriteSession;
	QUuid mainReadSessionId;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCSESSIONSTORAGE_H
