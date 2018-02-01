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

#ifndef ARPCCONFIG_H
#define ARPCCONFIG_H

#include <QString>

class ARpcConfig
{
public:
	static const QByteArray funcCallMsg;
	static const QByteArray funcAnswerOkMsg;
	static const QByteArray funcAnswerErrMsg;
	static const QByteArray funcSyncMsg;
	static const QByteArray infoMsg;
	static const QByteArray measurementMsg;
	static const QByteArray identifyMsg;
	static const QByteArray identifyHubMsg;
	static const QByteArray deviceInfoMsg;
	static const QByteArray stateChangedMsg;
	static const QByteArray hubMsg;
//	static const QString queryVersionMsg;
//	static const QString versionMsg;
	static const char msgDelim;
	static const char argDelim;
	static const int identifyWaitTime;//msecs
	static const int syncCallWaitTime;//msecs
	static const QByteArray serverReadyMsg;

	//reserved commands
	static const QByteArray getSensorsCommand;
	static const QByteArray getControlsCommand;
	static const QByteArray getStateCommand;

	static const quint16 netDevicePort;
	static const quint16 netDeviceSslPort;
};

#endif // ARPCCONFIG_H
