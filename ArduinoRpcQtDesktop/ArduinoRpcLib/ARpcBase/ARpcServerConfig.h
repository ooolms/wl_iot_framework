#ifndef ARPCSERVERCONFIG_H
#define ARPCSERVERCONFIG_H

#include <QString>

class ARpcServerConfig
{
public:
	static const QByteArray authentificateSrvMsg;
	static const QByteArray srvCmdDataMsg;
	static const QByteArray notifyDeviceIdentifiedMsg;
	static const QByteArray notifyDeviceLostMsg;
	static const QByteArray notifyStorageCreatedMsg;
	static const QByteArray notifyStorageRemovedMsg;
	static const quint16 controlSslPort;
};

#endif // ARPCSERVERCONFIG_H
