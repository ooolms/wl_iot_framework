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

#include "wliot/WLIOTConfig.h"

const QByteArray WLIOTConfig::funcCallMsg="call";
const QByteArray WLIOTConfig::funcAnswerErrMsg="err";
const QByteArray WLIOTConfig::funcAnswerOkMsg="ok";
const QByteArray WLIOTConfig::funcSynccMsg="syncc";
const QByteArray WLIOTConfig::devSyncMsg="sync";
const QByteArray WLIOTConfig::devSyncrMsg="syncr";
const QByteArray WLIOTConfig::measurementMsg="meas";
const QByteArray WLIOTConfig::measurementBMsg="measb";
const QByteArray WLIOTConfig::infoMsg="info";
const QByteArray WLIOTConfig::identifyMsg="identify";
const QByteArray WLIOTConfig::identifyHubMsg="identify_hub";
const QByteArray WLIOTConfig::deviceInfoMsg="deviceinfo";
const QByteArray WLIOTConfig::stateChangedMsg="statechanged";
const QByteArray WLIOTConfig::hubMsg="#hub";
const QByteArray WLIOTConfig::hubDeviceIdentifiedMsg="device_identified";
const QByteArray WLIOTConfig::hubDeviceLostMsg="device_lost";
const QByteArray WLIOTConfig::serverReadyMsg="server_ready";
//const QString ARpcConfig::queryVersionMsg=QString("queryversion");
//const QString ARpcConfig::versionMsg=QString("version");
const char WLIOTConfig::msgDelim=10;//\n
const char WLIOTConfig::argDelim=124;//|
const int WLIOTConfig::synccCallWaitTime=5000;
const int WLIOTConfig::identifyWaitTime=6000;

const QByteArray WLIOTConfig::getSensorsCommand="#sensors";
const QByteArray WLIOTConfig::getControlsCommand="#controls";
const QByteArray WLIOTConfig::getStateCommand="#state";

const quint16 WLIOTConfig::netDevicePort=4081;
const quint16 WLIOTConfig::netDeviceSslPort=4082;
