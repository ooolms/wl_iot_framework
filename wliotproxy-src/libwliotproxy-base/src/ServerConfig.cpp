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

#include "wliot/ServerConfig.h"

const QByteArray ServerConfig::srvCmdDataMsg="cmdata";
const QByteArray ServerConfig::authenticateSrvMsg="authenticate";
const QByteArray ServerConfig::notifyDeviceIdentifiedMsg="device_identified";
const QByteArray ServerConfig::notifyDeviceLostMsg="device_lost";
const QByteArray ServerConfig::notifyStorageCreatedMsg="storage_created";
const QByteArray ServerConfig::notifyStorageRemovedMsg="storage_removed";
const QByteArray ServerConfig::notifyReloadDevsAndStorsMsg="reload_devices_and_storages";
const quint16 ServerConfig::controlSslPort=4083;
