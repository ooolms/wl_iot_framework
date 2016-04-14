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
	QString boardResetMsgTitle;
	QString infoMsgTitle;
};

#endif // ARPCCONFIG_H
