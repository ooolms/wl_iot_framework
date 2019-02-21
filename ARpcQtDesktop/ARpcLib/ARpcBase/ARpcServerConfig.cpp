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

#include "ARpcServerConfig.h"

const QByteArray ARpcServerConfig::srvCmdDataMsg="cmdata";
const QByteArray ARpcServerConfig::authenticateSrvMsg="authenticate";
const QByteArray ARpcServerConfig::notifyDeviceIdentifiedMsg="device_identified";
const QByteArray ARpcServerConfig::notifyDeviceLostMsg="device_lost";
const QByteArray ARpcServerConfig::notifyStorageCreatedMsg="storage_created";
const QByteArray ARpcServerConfig::notifyStorageRemovedMsg="storage_removed";
const QByteArray ARpcServerConfig::notifyReloadDevsAndStorsMsg="reload_devices_and_storages";
const quint16 ARpcServerConfig::controlSslPort=4083;
