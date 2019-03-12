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

#include "wliot/WLIOTProtocolDefs.h"

const QByteArray WLIOTProtocolDefs::funcCallMsg="call";
const QByteArray WLIOTProtocolDefs::funcAnswerErrMsg="err";
const QByteArray WLIOTProtocolDefs::funcAnswerOkMsg="ok";
const QByteArray WLIOTProtocolDefs::funcSynccMsg="syncc";
const QByteArray WLIOTProtocolDefs::devSyncMsg="sync";
const QByteArray WLIOTProtocolDefs::devSyncrMsg="syncr";
const QByteArray WLIOTProtocolDefs::measurementMsg="meas";
const QByteArray WLIOTProtocolDefs::measurementBMsg="measb";
const QByteArray WLIOTProtocolDefs::infoMsg="info";
const QByteArray WLIOTProtocolDefs::identifyMsg="identify";
const QByteArray WLIOTProtocolDefs::identifyHubMsg="identify_hub";
const QByteArray WLIOTProtocolDefs::deviceInfoMsg="deviceinfo";
const QByteArray WLIOTProtocolDefs::stateChangedMsg="statechanged";
const QByteArray WLIOTProtocolDefs::hubMsg="#hub";
const QByteArray WLIOTProtocolDefs::hubDeviceIdentifiedMsg="device_identified";
const QByteArray WLIOTProtocolDefs::hubDeviceLostMsg="device_lost";
const QByteArray WLIOTProtocolDefs::serverReadyMsg="server_ready";
//const QString WLIOTConfig::queryVersionMsg=QString("queryversion");
//const QString WLIOTConfig::versionMsg=QString("version");
const char WLIOTProtocolDefs::msgDelim=10;//\n
const char WLIOTProtocolDefs::argDelim=124;//|
const int WLIOTProtocolDefs::synccCallWaitTime=5000;
const int WLIOTProtocolDefs::identifyWaitTime=6000;

const QByteArray WLIOTProtocolDefs::getSensorsCommand="#sensors";
const QByteArray WLIOTProtocolDefs::getControlsCommand="#controls";
const QByteArray WLIOTProtocolDefs::getStateCommand="#state";

const quint16 WLIOTProtocolDefs::netDevicePort=4081;
const quint16 WLIOTProtocolDefs::netDeviceSslPort=4082;
