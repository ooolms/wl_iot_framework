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

#ifndef PROGRAMSTORAGESRUNTIME_H
#define PROGRAMSTORAGESRUNTIME_H

#include "VDIL/core/IProgramRuntime.h"
#include "wliot/storages/StorageId.h"
#include <QMap>

namespace WLIOTVDIL
{
	class Program;
	class StorageSourceBlock;

	class ProgramStoragesRuntimeInstance
		:public IProgramRuntimeInstance
	{
		Q_OBJECT
	public:
		explicit ProgramStoragesRuntimeInstance(Program *p);
		virtual void prepareToStart()override;
		virtual void cleanupAfterStop()override;

	private slots:
		void onNewValue();

	private:
		QMap<WLIOT::StorageId,QList<StorageSourceBlock*>> mBlocksMap;
		QMap<WLIOT::StorageId,QMetaObject::Connection> mConnMap;
	};

	class ProgramStoragesRuntime
		:public IProgramRuntime
	{
	public:
		virtual IProgramRuntimeInstance *mkRuntime(Program *p)override;
	};
}

#endif // PROGRAMSTORAGESRUNTIME_H
