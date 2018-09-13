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

#ifndef ARPCSERVERCONFIG_H
#define ARPCSERVERCONFIG_H

#include <QString>

class ARpcServerConfig
{
public:
	static const QByteArray authentificateSrvMsg;
	static const QByteArray srvCmdDataMsg;
	static const QByteArray notifyDeviceIdentifiedMsg;
	static const QByteArray notifyDeviceLostMsg;
	static const QByteArray notifyStorageCreatedMsg;
	static const QByteArray notifyStorageRemovedMsg;
	static const quint16 controlSslPort;
};

#endif // ARPCSERVERCONFIG_H
