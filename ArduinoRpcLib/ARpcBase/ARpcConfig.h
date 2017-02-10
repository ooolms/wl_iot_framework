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
	static const QString deviceInfoMsg;
	static const QChar msgDelim;
	static const QChar argDelim;
	static const int identifyWaitTime;//msecs
	static const int syncCallWaitTime;//msecs

	//reserved commands
	static const QString getSensorsCommand;
	static const QString getControlsCommand;
};

#endif // ARPCCONFIG_H
