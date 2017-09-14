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
	static const QString funcCallMsg;
	static const QString funcAnswerOkMsg;
	static const QString funcAnswerErrMsg;
	static const QString funcSyncMsg;
	static const QString infoMsg;
	static const QString measurementMsg;
	static const QString identifyMsg;
	static const QString identifyHubMsg;
	static const QString deviceInfoMsg;
	static const QString stateChangedMsg;
	static const QString hubMsg;
//	static const QString queryVersionMsg;
//	static const QString versionMsg;
	static const QChar msgDelim;
	static const QChar argDelim;
	static const int identifyWaitTime;//msecs
	static const int syncCallWaitTime;//msecs
	static const QString serverReadyMsg;

	//reserved commands
	static const QString getSensorsCommand;
	static const QString getControlsCommand;
	static const QString getStateCommand;

	static const quint16 netDevicePort;
	static const quint16 netDeviceSslPort;
};

#endif // ARPCCONFIG_H
