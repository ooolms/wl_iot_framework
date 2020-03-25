#ifndef PROGRAM_H
#define PROGRAM_H

#include "wliot/storages/StorageId.h"
#include "GIDL/core/BaseBlock.h"
#include "GIDL/core/SourceBlock.h"
#include <QUuid>
#include <QList>
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
	bool extractSources();//call 1-st from main thread
	bool prepareWorkData();//call 2-nd from work thread
	bool eval();//call 3-rd from work thread

public:
	QList<QUuid> deviceTriggers;
	QList<StorageId> sensorTriggers;
	QList<SourceBlock*> sourceBlocks;
	QList<BaseBlock*> processingBlocks;
	QMutex nextDataLock;
};

#endif // PROGRAM_H
