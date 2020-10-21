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

#ifndef PROGRAMDEVICESRUNTIME_H
#define PROGRAMDEVICESRUNTIME_H

#include "VDIL/core/IProgramRuntime.h"
#include <QUuid>
#include <QMap>

namespace WLIOTVDIL
{
	class Program;
	class DeviceStateSourceBlock;
	class DevicePresenceSourceBlock;

	class ProgramDevicesRuntimeInstance
		:public IProgramRuntimeInstance
	{
		Q_OBJECT
	public:
		explicit ProgramDevicesRuntimeInstance(Program *p);
		virtual void prepareToStart()override;
		virtual void cleanupAfterStop()override;

	private slots:
		void onDeviceConnectedAndDisconnected();
		void onDeviceStateChanged(const QByteArrayList &args);

	private:
		QMap<QUuid,QList<DeviceStateSourceBlock*>> mDevStateBlocksMap;
		QMap<QUuid,QList<DevicePresenceSourceBlock*>> mDevPresenceBlocksMap;
		QMap<QUuid,QMetaObject::Connection> mDevStateConnMap;
		QMap<QUuid,QPair<QMetaObject::Connection,QMetaObject::Connection>> mDevPresenceConnMap;
	};

	class ProgramDevicesRuntime
		:public IProgramRuntime
	{
	public:
		virtual IProgramRuntimeInstance *mkRuntime(Program *p)override;
	};
}

#endif // PROGRAMDEVICESRUNTIME_H
