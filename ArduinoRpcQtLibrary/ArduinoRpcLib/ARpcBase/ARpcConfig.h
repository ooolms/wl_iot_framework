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
	static const QString srvCmdDataMsg;
	static const QString measurementMsg;
	static const QString identifyMsg;
	static const QString deviceInfoMsg;
	static const QString stateChangedMsg;
//	static const QString queryVersionMsg;
//	static const QString versionMsg;
	static const QChar msgDelim;
	static const QChar argDelim;
	static const int identifyWaitTime;//msecs
	static const int syncCallWaitTime;//msecs

	//reserved commands
	static const QString getSensorsCommand;
	static const QString getControlsCommand;
	static const QString getStateCommand;
};

#endif // ARPCCONFIG_H
