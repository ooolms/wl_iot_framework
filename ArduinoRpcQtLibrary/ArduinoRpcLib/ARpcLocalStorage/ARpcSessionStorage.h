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
	Q_OBJECT
public:
	explicit ARpcSessionStorage(bool autoSess,ARpcSensor::Type valType,QObject *parent=0);
	virtual StoreMode getStoreMode()const;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	bool createAsFixedBlocksDb(const QVector<quint32> &blockNotesSizes,ARpcISensorStorage::TimestampRule rule);
	bool createAsChainedBlocksDb(ARpcISensorStorage::TimestampRule rule);
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	bool listSessions(QList<QUuid> &ids,QStringList &titles);
	bool createSession(const QString &title,QUuid &id);
	bool openSession(const QUuid &id);
	bool closeSession();
	bool removeSession(const QUuid &id);
	bool setSessionAttribute(const QString &key,const QVariant &val);
	bool getSessionAttribute(const QString &key,QVariant &val);
	qint64 valuesCount();
	ARpcISensorValue* valueAt(quint64 index);

protected:
	virtual bool openInternal()override;
	virtual void closeInternal()override;
	virtual ARpcSensor::Type effectiveValuesType()const;

private:
	QString blockNoteSizesToString();
	bool parseBlockNoteSizes(const QString &str);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	ARpcDBDriverHelpers hlp;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	QVector<quint32> blockNoteSizesForSessions;
	bool autoSessions;
	bool opened;
	bool sessionOpened;
	QUuid currentSessionId;
	QMap<QString,QVariant> sessionAttrs;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCSESSIONSTORAGE_H
