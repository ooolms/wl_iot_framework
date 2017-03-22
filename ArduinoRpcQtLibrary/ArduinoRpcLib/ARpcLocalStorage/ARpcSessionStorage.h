#ifndef ARPCSESSIONSTORAGE_H
#define ARPCSESSIONSTORAGE_H

#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
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
	explicit ARpcSessionStorage(bool autoSess,QObject *parent=0);
	virtual StoreMode getStoreMode()const;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	bool createAsFixedBlocksDb(const QVector<quint32> &blockNotesSizes);
	bool createAsChainedBlocksDb();
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	//TODO createSession,removeSession,openSession,closeSession,writeSessionAttributes
	bool listSessions(QList<QUuid> &ids,QStringList &titles);
	bool createSession(const QString &title,QUuid &id);
	bool openSession(const QUuid &id);
	bool closeSession();
	bool removeSession(const QUuid &id);

protected:
	virtual bool openInternal()override;
	virtual void closeInternal()override;

private:
	bool writeSensorValueFB(const ARpcISensorValue *val);
	bool writeSensorValueCB(const ARpcISensorValue *val);
	QString blockNoteSizesToString();
	bool parseBlockNoteSizes(const QString &str);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	QVector<quint32> blockNoteSizesForSessions;
	bool autoSessions;
	bool opened;
	bool sessionOpened;
	QUuid currentSessionId;
};

#endif // ARPCSESSIONSTORAGE_H
