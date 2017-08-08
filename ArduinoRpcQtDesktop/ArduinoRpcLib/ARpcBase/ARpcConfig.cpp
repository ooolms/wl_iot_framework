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

const QString ARpcConfig::funcCallMsg=QString("call");
const QString ARpcConfig::funcAnswerErrMsg=QString("err");
const QString ARpcConfig::funcAnswerOkMsg=QString("ok");
const QString ARpcConfig::funcSyncMsg=QString("sync");
const QString ARpcConfig::measurementMsg=QString("meas");
const QString ARpcConfig::infoMsg=QString("info");
const QString ARpcConfig::identifyMsg=QString("identify");
const QString ARpcConfig::identifyHubMsg=QString("identify_hub");
const QString ARpcConfig::deviceInfoMsg=QString("deviceinfo");
const QString ARpcConfig::srvCmdDataMsg=QString("cmdata");
const QString ARpcConfig::stateChangedMsg=QString("statechanged");
const QString ARpcConfig::hubMsg=QString("#hub");
//const QString ARpcConfig::queryVersionMsg=QString("queryversion");
//const QString ARpcConfig::versionMsg=QString("version");
const QChar ARpcConfig::msgDelim=QChar('\n');
const QChar ARpcConfig::argDelim=QChar('|');
const int ARpcConfig::syncCallWaitTime=2000;
const int ARpcConfig::identifyWaitTime=6000;

const QString ARpcConfig::getSensorsCommand=QString("#sensors");
const QString ARpcConfig::getControlsCommand=QString("#controls");
const QString ARpcConfig::getStateCommand=QString("#state");
