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
#include "VDIL/core/BaseBlock.h"
#include "VDIL/core/SourceBlock.h"
#include "VDIL/core/IEngineHelper.h"
#include "VDIL/core/IEngineCallbacks.h"
#include "VDIL/core/TimerBlock.h"
#include "VDIL/core/ITrigger.h"
#include <QUuid>
#include <QList>
#include <QSet>
#include <QMutex>
#include <QTimer>

namespace WLIOTVDIL
{
	//TODO оптимизация - при срабатывании триггера извлекать новые данные только для соответствующих блоков,
	//например только для блоков хранилища, в котором появились новые данные

	struct ConfigOptionId
	{
		quint32 blockId;
		QByteArray key;

		bool operator<(const ConfigOptionId &t)const;
		bool operator==(const ConfigOptionId &t)const;
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
		QSet<ITrigger*> mkTriggers();

		//blocks manipulation
		bool addBlock(BaseBlock *b);
		void rmBlock(quint32 bId);
		BaseBlock* blockById(quint32 bId);
		const QMap<quint32,BaseBlock*>& allBlocks()const;
		const QMap<quint32,SourceBlock*>& sourceBlocks()const;
		const QMap<quint32,TimerBlock*>& timerBlocks()const;

		//config options
		const QList<ConfigOptionId>& allConfigOptions()const;
		DataUnit configOptionValue(ConfigOptionId id)const;
		bool setConfigOptionValue(ConfigOptionId id,const DataUnit &val);

	private:
		bool extractSources();//call 1-st from main thread
		bool prepareWorkData();//call 2-nd from work thread
		bool eval();//call 3-rd from work thread
		void cleanUpAfterEval();
		void calcConfigOptions();
		void addEvalTimer(QTimer *t);
		void rmEvalTimer(QTimer *t);
		void updateExtTimersList();
		void setHelper(IEngineHelper *h);
		void setEngineCallbacks(IEngineCallbacks *c);
		IEngineHelper* helper()const;
		IEngineCallbacks* engineCallbacks()const;

	private://store to xml
		QMap<quint32,BaseBlock*> mAllBlocks;

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
}

#endif // PROGRAM_H