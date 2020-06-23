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

#include "StandardErrors.h"

QByteArray StandardErrors::invalidAgruments()
{
	return "invaid arguments";
}

QByteArray StandardErrors::noDeviceFound(const QByteArray &devIdOrName)
{
	return "device is not found or more than 1 found with equal names: "+devIdOrName;
}

QByteArray StandardErrors::deviceNotIdentified(const QByteArray &devAddress)
{
	return "device is not identified "+devAddress;
}

QByteArray StandardErrors::cantWriteDevicesConfig()
{
	return "can't write devices configuration";
}

QByteArray StandardErrors::sessionNotFound(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId)
{
	return "session "+sessionId.toByteArray()+" not found for storage "+devIdOrName+":"+sensorName;
}

QByteArray StandardErrors::storageNotFound(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return "storage "+devIdOrName+":"+sensorName+" not found";
}

QByteArray StandardErrors::unknownCommand(const QByteArray &cmd)
{
	return "unknown command: "+cmd;
}

QByteArray StandardErrors::noUserFound(const QByteArray &userNameOrId)
{
	return "no user found: "+userNameOrId;
}

QByteArray StandardErrors::accessDenied()
{
	return "access denied";
}

QByteArray StandardErrors::someStrangeError()
{
	return "some strange error happened";
}
