#ifndef ARPCSERVERCONFIG_H
#define ARPCSERVERCONFIG_H

#include <QString>

class ARpcServerConfig
{
public:
	static const QString authentificateSrvMsg;
	static const QString srvCmdDataMsg;
	static const QString notifyDeviceIdentifiedMsg;
	static const QString notifyDeviceLostMsg;
	static const QString notifyStorageCreatedMsg;
	static const QString notifyStorageRemovedMsg;
	static const quint16 controlSslPort;
};

#endif // ARPCSERVERCONFIG_H
