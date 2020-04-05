#ifndef PROGRAM_H
#define PROGRAM_H

#include "wliot/storages/StorageId.h"
#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/SourceBlock.h"
#include "GDIL/core/IEngineHelper.h"
#include "GDIL/core/ICommandCallback.h"
#include <QUuid>
#include <QList>
#include <QSet>
#include <QMutex>

//TODO оптимизация - при срабатывании триггера извлекать новые данные только для соответствующих блоков,
//например только для блоков хранилища, в котором появились новые данные

/*
 * В основном потоке вызываем extractSources, затем в рабочем prepareWorkData и eval.
 * При этом за время выполнения программы могут сработать несколько триггеров,
 * и несколько раз вызовется extract, но prepare и eval нужно вызывать через очередь сообщений
 * (использовать флаг типа msgSendToLoop, чтобы не запихивать в очередь сразу несколько сообщений,
 * тогда на много триггеров программа будет выполнена только один раз, но с самыми новыми данными)
*/
class Program
{
public:
	Program();
	~Program();
	void setHelper(IEngineHelper *h);
	IEngineHelper* helper();
	void setCommandCallback(ICommandCallback *c);
	ICommandCallback* commandCallback();
	bool extractSources();//call 1-st from main thread
	bool prepareWorkData();//call 2-nd from work thread
	bool eval();//call 3-rd from work thread
	bool addSourceBlock(SourceBlock *b);
	bool addProcessingBlock(BaseBlock *b);
	void rmBlock(quint32 bId);
	BaseBlock* blockById(quint32 bId);
	const QMap<quint32,SourceBlock*>& sourceBlocks()const;
	const QMap<quint32,BaseBlock*>& processingBlocks()const;

public:
	QList<QUuid> deviceTriggers;
	QList<StorageId> storageTriggers;

private:
	QMap<quint32,SourceBlock*> mSourceBlocks;
	QMap<quint32,BaseBlock*> mProcessingBlocks;
	QMutex nextDataLock;
	IEngineHelper *hlp;
	ICommandCallback *cmdCb;
	quint32 maxBlockId;
};

#endif // PROGRAM_H
