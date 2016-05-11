#ifndef ARPCCONFIG_H
#define ARPCCONFIG_H

#include <QString>

class ARpcConfig
{
public:
	ARpcConfig();

public:
	QString funcCallOkMsgTitle;
	QString funcCallErrMsgTitle;
	QString funcCallSyncMsgTitle;
	QString infoMsgTitle;
	QString measurementMsgTitle;
	QChar msgDelim;
	QChar argDelim;
};

#endif // ARPCCONFIG_H
