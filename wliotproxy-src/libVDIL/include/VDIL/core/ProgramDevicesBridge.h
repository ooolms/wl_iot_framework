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

#ifndef PROGRAMDEVICESBRIDGE_H
#define PROGRAMDEVICESBRIDGE_H

#include <QObject>
#include <QUuid>
#include <QMap>

namespace WLIOTVDIL
{
	class Program;
	class DeviceStateSourceBlock;
	class DevicePresenceSourceBlock;

	class ProgramDevicesBridge
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ProgramDevicesBridge(Program *p);
		void prepareToStart();
		void cleanupAfterStop();

	signals:
		void activateProgram();

	private slots:
		void onDeviceConnectedAndDisconnected();
		void onDeviceStateChanged();

	private:
		Program *prg;
		QMap<QUuid,QList<DeviceStateSourceBlock*>> mDevStateBlocksMap;
		QMap<QUuid,QList<DevicePresenceSourceBlock*>> mDevPresenceBlocksMap;
		QMap<QUuid,QMetaObject::Connection> mDevStateConnMap;
		QMap<QUuid,QPair<QMetaObject::Connection,QMetaObject::Connection>> mDevPresenceConnMap;
	};
}

#endif // PROGRAMDEVICESBRIDGE_H
