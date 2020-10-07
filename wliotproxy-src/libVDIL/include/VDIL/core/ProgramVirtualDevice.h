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

#ifndef PROGRAMVIRTUALDEVICE_H
#define PROGRAMVIRTUALDEVICE_H

#include "wliot/devices/SensorDef.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/VirtualDeviceBackend.h"
#include <QUuid>

//TODO add typeId
namespace WLIOTVDIL
{
	class Program;
	class VDevCommandSourceBlock;

	class ProgramVirtualDevice
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ProgramVirtualDevice(Program *p);

		//configuration
		void setSensors(const QList<WLIOT::SensorDef> &sensors);
		const QList<WLIOT::SensorDef>& sensors()const;
		WLIOT::SensorDef sensorByName(const QByteArray &name)const;
		void setControls(const WLIOT::ControlsGroup &controls);
		const WLIOT::ControlsGroup& controls()const;
		const QMap<QByteArray,WLIOT::ControlsCommand>& commandsMap()const;
		void setParams(bool enabled,const QUuid &id,const QByteArray &devName,const QUuid &typeId=QUuid());
		QUuid devId()const;
		QByteArray devName()const;
		QUuid typeId()const;
		bool enabled()const;
		const WLIOT::DeviceState& state()const;

		//runtime
		void prepareToStart();
		void cleanupAfterStop();
		bool onCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal);

	signals:
		void activateProgram();

	private:
		Program *prg;
		bool mEnabled;
		QUuid mDevId,mTypeId;
		QByteArray mDevName;
		QList<WLIOT::SensorDef> mSensors;
		WLIOT::ControlsGroup mControls;

		//runtime
		QMap<QByteArray,WLIOT::ControlsCommand> mCommands;
		WLIOT::DeviceState mState;
		QMap<QByteArray,QList<VDevCommandSourceBlock*>> mCmdBlocksMap;
	};
}

#endif // PROGRAMVIRTUALDEVICE_H
