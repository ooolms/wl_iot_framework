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

#ifndef SESSIONSTORAGE_H
#define SESSIONSTORAGE_H

#include "wliot/storages/BaseFSSensorStorage.h"
#include "wliot/storages/ISessionSensorStorage.h"
#include <QUuid>

namespace WLIOT
{
	class DBDriverFixedBlocks;
	class DBDriverChainedBlocks;
	class DBDriverHelpers;
	class DBDriverGTimeIndex;
	class FSSensorStorageHelper;

	/**
	 * @brief The ARpcSessionStorage class
	 * Сессии хранятся в поддиректории "sessions" основной директории базы.
	 * Каждая сессия - папка с названием в виде UID ("{00000000-0000-0000-0000-000000000000}").
	 * В папке лежит отдельный файл с названием сессии "title.txt",
	 * ini-файл с пользовательскими атрибутами и сама база со значениями.
	 */
	class SessionStorage
		:public ISessionSensorStorage
	{
	private:
		struct Session
		{
			union
			{
				DBDriverFixedBlocks *fbDb;
				DBDriverChainedBlocks *cbDb;
			};

			DBDriverGTimeIndex *indDb;
			QMap<QByteArray,QByteArray> attributes;
		};

		Q_OBJECT

	public:
		explicit SessionStorage(const QString &path,bool autoSess,const QUuid &devId,const QByteArray &devName,
			const SensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
		virtual ~SessionStorage();
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
		virtual SensorValue* valueAt(const QUuid &sessionId,quint64 index)override;
		virtual bool isSessionOpened(const QUuid &sessionId)const override;
		virtual bool openMainWriteSession(const QUuid &sessionId)override;
		virtual bool isMainWriteSessionOpened()const override;
		virtual bool closeMainWriteSession()override;
		virtual QUuid getMainWriteSessionId()const override;
		virtual bool setMainReadSessionId(const QUuid &id)override;
		virtual void close()override;
		virtual void writeAttribute(const QByteArray &str, const QByteArray &var)override;
		virtual QByteArray readAttribute(const QByteArray &str)override;
		virtual void addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)override;
		virtual bool hasDataExportConfig(const QUuid &serviceId)override;
		virtual DataExportConfig getDataExportConfig(const QUuid &serviceId)override;
		virtual void removeDataExportConfig(const QUuid &serviceId)override;
		virtual QList<QUuid> allDataExportServices()override;
		virtual bool values(quint64 index,quint64 count,quint64 step,
			VeryBigArray<SensorValue*> &vals)override;
		using ISessionSensorStorage::values;
		virtual bool hasGTIndex()override;
		virtual quint64 findInGTIndex(qint64 ts)override;
		virtual quint64 findInGTIndex(const QUuid &sessionId,qint64 ts)override;

	public:
		virtual bool open()override;
		virtual bool writeSensorValue(const SensorValue *val)override;
		virtual bool isOpened()const override;
		virtual quint64 valuesCount()override;
		virtual SensorValue* valueAt(quint64 index)override;

	private:
		void closeSessionAndDeleteDb(Session &d);
		bool createAsFixedBlocksDb(bool gtIndex);
		bool createAsChainedBlocksDb(bool gtIndex);

	private:
		FSSensorStorageHelper *fsStorageHelper;
		DBDriverHelpers *hlp;
		QMap<QUuid,Session> sessions;
		enum
		{
			FIXED_BLOCKS,
			CHAINED_BLOCKS
		} dbType;

		bool autoSessions;
		bool opened;
		bool mHasGTIndex;
		QUuid mainWriteSessionId;
		Session mainWriteSession;
		QUuid mainReadSessionId;
	};
}

#endif // SESSIONSTORAGE_H
