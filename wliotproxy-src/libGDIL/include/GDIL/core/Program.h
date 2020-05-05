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
#include "GDIL/core/TimerBlock.h"
#include <QUuid>
#include <QList>
#include <QSet>
#include <QMutex>
#include <QTimer>

//TODO оптимизация - при срабатывании триггера извлекать новые данные только для соответствующих блоков,
//например только для блоков хранилища, в котором появились новые данные

struct ConfigOptionId
{
	quint32 blockId;
	QByteArray key;

	bool operator<(const ConfigOptionId &t)const;
};

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
	QString findDevName(const QUuid &id)const;

	//blocks manipulation
	bool addBlock(BaseBlock *b);
	void rmBlock(quint32 bId);
	BaseBlock* blockById(quint32 bId);
	const QMap<quint32,BaseBlock*>& allBlocks()const;
	const QMap<quint32,SourceBlock*>& sourceBlocks()const;
	const QMap<quint32,TimerBlock*>& timerBlocks()const;

	//triggers
	const QList<QUuid>& deviceTriggers()const;
	const QList<StorageId>& storageTriggers()const;
	void setStorageTriggers(const QList<StorageId> &list);
	QList<StorageId> allUsedStorages()const;

	//config options
	QList<ConfigOptionId> allConfigOptions() const;
	TypeConstraints configOptionConstraints(ConfigOptionId id)const;
	DataUnit configOptionValue(ConfigOptionId id)const;
	bool setConfigOptionValue(ConfigOptionId id,const DataUnit &val);

private:
	bool extractSources();//call 1-st from main thread
	bool prepareWorkData();//call 2-nd from work thread
	bool eval();//call 3-rd from work thread
	void calcConfigOptions();
	void addEvalTimer(QTimer *t);
	void rmEvalTimer(QTimer *t);
	void updateExtTimersList();
	void updateDevNames();
	void setHelper(IEngineHelper *h);
	void setEngineCallbacks(IEngineCallbacks *c);
	IEngineHelper* helper()const;
	IEngineCallbacks* engineCallbacks()const;

private://store to xml
	QMap<quint32,BaseBlock*> mAllBlocks;
	QList<StorageId> mStorageTriggers;
	QMap<QUuid,QString> mKnownDevNames;

private://runtime
	QMap<quint32,SourceBlock*> mSourceBlocks;
	QMap<quint32,TimerBlock*> mTimerBlocks;
	QSet<QTimer*> evalTimers;
	QList<ConfigOptionId> mAllConfigOptions;
	QMap<quint32,TimerBlock::TimerConfig> mExtTimerConfigs;
	QMutex nextDataLock;
	IEngineHelper *hlp;
	IEngineCallbacks *mCb;
	quint32 maxBlockId;
	QList<QUuid> mDeviceTriggers;
	friend class ProgramObject;
	friend class BaseBlock;
	friend class ProgramXmlParser;
};

#endif // PROGRAM_H
