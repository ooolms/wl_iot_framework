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

#ifndef STANDARDERRORS_H
#define STANDARDERRORS_H

#include <QString>
#include "wliot/storages/StorageId.h"

class StandardErrors
{
public:
	static QByteArray invalidAgruments();
	static QByteArray noDeviceFound(const QByteArray &devIdOrName);
	static QByteArray deviceNotIdentified(const QByteArray &devAddress);
	static QByteArray cantWriteDevicesConfig();
	static QByteArray sessionNotFound(const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId);
	static QByteArray storageNotFound(const QByteArray &devIdOrName,const QByteArray &sensorName);
	static QByteArray unknownCommand(const QByteArray &cmd);
	static QByteArray noUserFound(const QByteArray &userNameOrId);
	static QByteArray accessDenied();

	static QByteArray someStrangeError();
};

#endif // STANDARDERRORS_H
