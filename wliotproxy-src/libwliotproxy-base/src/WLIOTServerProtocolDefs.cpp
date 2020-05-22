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

#include "wliot/WLIOTServerProtocolDefs.h"

using namespace WLIOT;

const QByteArray WLIOTServerProtocolDefs::srvCmdDataMsg="cmdata";
const QByteArray WLIOTServerProtocolDefs::authenticateSrvMsg="authenticate";
const QByteArray WLIOTServerProtocolDefs::notifyDeviceIdentifiedMsg="device_identified";
const QByteArray WLIOTServerProtocolDefs::notifyDeviceLostMsg="device_lost";
const QByteArray WLIOTServerProtocolDefs::notifyStorageCreatedMsg="storage_created";
const QByteArray WLIOTServerProtocolDefs::notifyStorageRemovedMsg="storage_removed";
const QByteArray WLIOTServerProtocolDefs::notifyReloadDevsAndStorsMsg="reload_devices_and_storages";
const QByteArray WLIOTServerProtocolDefs::vdevMsg="vdev";
const quint16 WLIOTServerProtocolDefs::controlSslPort=4083;
