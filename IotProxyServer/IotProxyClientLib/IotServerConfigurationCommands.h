#ifndef IOTSERVERCONFIGURATIONCOMMANDS_H
#define IOTSERVERCONFIGURATIONCOMMANDS_H

#include <QObject>
#include "IotServerConnection.h"

class IotServerConfigurationCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerConfigurationCommands(IotServerConnection *conn,QObject *parent=nullptr);
	bool getTtyFilterByPortName(QByteArray &filter);
	bool setTtyFilterByPortName(const QByteArray &filter);
	bool getTtyFilterByPidVid(QByteArray &filter);
	bool setTtyFilterByPidVid(const QByteArray &filter);
	bool getTcpFilter(QByteArray &filter);
	bool setTcpFilter(const QByteArray &filter);
	bool setDetectTcpDevices(bool enable);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERCONFIGURATIONCOMMANDS_H
