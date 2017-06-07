#ifndef ARPCSESSIONSTORAGE_H
#define ARPCSESSIONSTORAGE_H

#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
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
		QMap<QString,QVariant> attributes;
	};

	Q_OBJECT
public:
	explicit ARpcSessionStorage(bool autoSess,ARpcSensor::Type valType,QObject *parent=0);
	virtual ~ARpcSessionStorage();
	bool createAsFixedBlocksDb(const ARpcISensorValue &templateValue,ARpcISensorStorage::TimestampRule rule);
	bool createAsChainedBlocksDb(ARpcISensorStorage::TimestampRule rule);
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
	qint64 valuesCount(const QUuid &sessionId);
	ARpcISensorValue* valueAt(const QUuid &sessionId,quint64 index);
	bool isSessionOpened(const QUuid &sessionId)const;
	bool isMainWriteSessionOpened()const;
	virtual bool isOpened()const override;
	QUuid getMainWriteSessionId()const;

public:
	virtual bool open()override;
	virtual StoreMode getStoreMode()const;
	virtual ARpcSensor::Type effectiveValuesType()const;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;

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
	QUuid mainWriteSessionId;
	Session mainWriteSession;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCSESSIONSTORAGE_H
