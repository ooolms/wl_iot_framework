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

#ifndef WLIOTSERVERPROTOCOLDEFS_H
#define WLIOTSERVERPROTOCOLDEFS_H

#include <QString>

class WLIOTServerProtocolDefs
{
public:
	static const QByteArray authenticateSrvMsg;
	static const QByteArray srvCmdDataMsg;
	static const QByteArray notifyDeviceIdentifiedMsg;
	static const QByteArray notifyDeviceLostMsg;
	static const QByteArray notifyStorageCreatedMsg;
	static const QByteArray notifyStorageRemovedMsg;
	static const QByteArray notifyReloadDevsAndStorsMsg;
	static const QByteArray vdevMsg;
	static const quint16 controlSslPort;
};

#endif // WLIOTSERVERPROTOCOLDEFS_H
