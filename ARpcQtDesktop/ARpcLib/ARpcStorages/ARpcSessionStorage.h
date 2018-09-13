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

#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcISessionSensorStorage.h"
#include "ARpcStorages/ARpcDBDriverFixedBlocks.h"
#include "ARpcStorages/ARpcDBDriverChainedBlocks.h"
#include "ARpcStorages/ARpcDBDriverHelpers.h"
#include "ARpcStorages/ARpcDBDriverGTimeIndex.h"
#include "ARpcStorages/ARpcFSSensorStorageHelper.h"
#include <QUuid>

/**
 * @brief The ARpcSessionStorage class
 * Сессии хранятся в поддиректории "sessions" основной директории базы.
 * Каждая сессия - папка с названием в виде UID ("{00000000-0000-0000-0000-000000000000}").
 * В папке лежит отдельный файл с названием сессии "title.txt",
 * ini-файл с пользовательскими атрибутами и сама база со значениями.
 */
class ARpcSessionStorage
	:public ARpcISessionSensorStorage
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
		QMap<QByteArray,QByteArray> attributes;
	};

	Q_OBJECT

public:
	explicit ARpcSessionStorage(const QString &path,bool autoSess,const QUuid &devId,const QByteArray &devName,
		const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
	virtual ~ARpcSessionStorage();
	bool create(bool gtIndex=false);
	virtual bool listSessions(QList<QUuid> &ids,QByteArrayList &titles)override;
	virtual bool createSession(const QByteArray &title,QUuid &sessionId)override;
	virtual bool openSession(const QUuid &sessionId)override;
	virtual bool closeSession(const QUuid &sessionId)override;
	virtual bool removeSession(const QUuid &sessionId)override;
	virtual bool setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)override;
	virtual bool removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)override;
	virtual bool getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)override;
	virtual bool listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)override;
	virtual quint64 valuesCount(const QUuid &sessionId)override;
	virtual ARpcSensorValue* valueAt(const QUuid &sessionId,quint64 index)override;
	virtual bool isSessionOpened(const QUuid &sessionId)const override;
	virtual bool openMainWriteSession(const QUuid &sessionId)override;
	virtual bool isMainWriteSessionOpened()const override;
	virtual bool closeMainWriteSession()override;
	virtual QUuid getMainWriteSessionId()const override;
	virtual bool setMainReadSessionId(const QUuid &id)override;
	quint64 findInGTIndex(const QUuid &sessionId,qint64 ts);
	virtual void close()override;
	virtual void writeAttribute(const QByteArray &str, const QByteArray &var)override;
	virtual QByteArray readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)override;
	virtual void removeDataExportConfig(const QByteArray &serviceType)override;
	virtual QByteArrayList allDataExportServices()override;
	virtual bool values(quint64 index,quint64 count,quint64 step,
		VeryBigArray<ARpcSensorValue*> &vals)override;

public:
	virtual bool open()override;
	virtual bool writeSensorValue(const ARpcSensorValue *val)override;
	virtual bool isOpened()const override;
	virtual quint64 valuesCount()override;
	virtual ARpcSensorValue* valueAt(quint64 index)override;

private:
	void closeSessionAndDeleteDb(Session &d);
	bool createAsFixedBlocksDb(bool gtIndex);
	bool createAsChainedBlocksDb(bool gtIndex);

private:
	ARpcFSSensorStorageHelper fsStorageHelper;
	ARpcDBDriverHelpers hlp;
	QMap<QUuid,Session> sessions;
	enum
	{
		FIXED_BLOCKS,
		CHAINED_BLOCKS
	} dbType;

	bool autoSessions;
	bool opened;
	bool hasIndex;
	QUuid mainWriteSessionId;
	Session mainWriteSession;
	QUuid mainReadSessionId;
};

#endif // ARPCSESSIONSTORAGE_H
