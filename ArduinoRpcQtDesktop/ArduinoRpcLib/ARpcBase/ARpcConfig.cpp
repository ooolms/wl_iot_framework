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

#include "ARpcConfig.h"

const QByteArray ARpcConfig::funcCallMsg="call";
const QByteArray ARpcConfig::funcAnswerErrMsg="err";
const QByteArray ARpcConfig::funcAnswerOkMsg="ok";
const QByteArray ARpcConfig::funcSynccMsg="syncc";
const QByteArray ARpcConfig::devSyncMsg="sync";
const QByteArray ARpcConfig::devSyncrMsg="syncr";
const QByteArray ARpcConfig::measurementMsg="meas";
const QByteArray ARpcConfig::measurementFMsg="measf";
const QByteArray ARpcConfig::measurementGMsg="measg";
const QByteArray ARpcConfig::infoMsg="info";
const QByteArray ARpcConfig::identifyMsg="identify";
const QByteArray ARpcConfig::identifyHubMsg="identify_hub";
const QByteArray ARpcConfig::deviceInfoMsg="deviceinfo";
const QByteArray ARpcConfig::stateChangedMsg="statechanged";
const QByteArray ARpcConfig::hubMsg="#hub";
const QByteArray ARpcConfig::hubDeviceIdentifiedMsg="device_identified";
const QByteArray ARpcConfig::hubDeviceLostMsg="device_lost";
const QByteArray ARpcConfig::serverReadyMsg="server_ready";
//const QString ARpcConfig::queryVersionMsg=QString("queryversion");
//const QString ARpcConfig::versionMsg=QString("version");
const char ARpcConfig::msgDelim=10;//\n
const char ARpcConfig::argDelim=124;//|
const int ARpcConfig::synccCallWaitTime=5000;
const int ARpcConfig::identifyWaitTime=6000;

const QByteArray ARpcConfig::getSensorsCommand="#sensors";
const QByteArray ARpcConfig::getControlsCommand="#controls";
const QByteArray ARpcConfig::getStateCommand="#state";

const quint16 ARpcConfig::netDevicePort=4081;
const quint16 ARpcConfig::netDeviceSslPort=4082;
