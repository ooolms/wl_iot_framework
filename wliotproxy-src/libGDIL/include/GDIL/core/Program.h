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

#ifndef PROGRAM_H
#define PROGRAM_H

#include "wliot/storages/StorageId.h"
#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/SourceBlock.h"
#include "GDIL/core/IEngineHelper.h"
#include "GDIL/core/IEngineCallbacks.h"
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
	IEngineCallbacks* engineCallbacks();
	bool extractSources();//call 1-st from main thread
	bool prepareWorkData();//call 2-nd from work thread
	bool eval();//call 3-rd from work thread
	bool addBlock(BaseBlock *b);
	void rmBlock(quint32 bId);
	BaseBlock* blockById(quint32 bId);
	const QMap<quint32,SourceBlock*>& sourceBlocks()const;
	const QMap<quint32,BaseBlock*>& processingBlocks()const;
	const QList<QUuid>& deviceTriggers();
	const QList<StorageId>& storageTriggers();

private:
	void calcTriggers();
	void setEngineCallbacks(IEngineCallbacks *c);

private:
	QMap<quint32,SourceBlock*> mSourceBlocks;
	QMap<quint32,BaseBlock*> mProcessingBlocks;
	QMutex nextDataLock;
	IEngineHelper *hlp;
	IEngineCallbacks *mCb;
	quint32 maxBlockId;
	QList<QUuid> mDeviceTriggers;
	QList<StorageId> mStorageTriggers;
	friend class ProgramObject;
};

#endif // PROGRAM_H
