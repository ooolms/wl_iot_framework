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

#ifndef PROGRAMSTORAGESBRIDGE_H
#define PROGRAMSTORAGESBRIDGE_H

#include <QObject>
#include "wliot/storages/StorageId.h"
#include <QMap>

namespace WLIOTVDIL
{
	class Program;
	class StorageSourceBlock;

	class ProgramStoragesBridge
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ProgramStoragesBridge(Program *p);
		void prepareToStart();
		void cleanupAfterStop();

	signals:
		void activateProgram();

	private slots:
		void onNewValue();

	private:
		Program *prg;
		QMap<WLIOT::StorageId,QList<StorageSourceBlock*>> mBlocksMap;
		QMap<WLIOT::StorageId,QMetaObject::Connection> mConnMap;
	};
}

#endif // PROGRAMSTORAGESBRIDGE_H
