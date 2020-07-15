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

#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <QMap>
#include <QByteArrayList>
#include "wliot/devices/ControlsDefinition.h"

namespace WLIOT
{
	class DeviceState
	{
	public:
		QByteArrayList dumpToMsgArgs()const;
		bool parseMsgArgs(const QByteArrayList &args);
		static DeviceState makeFromCommands(const QList<ControlsCommand> &commands);

	public:
		QMap<QByteArray,QMap<quint32,QByteArray>> commandParams;
		QMap<QByteArray,QByteArray> additionalAttributes;
	};
}

#endif // DEVICESTATE_H
