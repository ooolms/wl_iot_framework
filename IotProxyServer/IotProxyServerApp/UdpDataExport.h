#ifndef UDPDATAEXPORT_H
#define UDPDATAEXPORT_H

#include <QUdpSocket>
#include <QMutex>
#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcMessageParser.h"

class UdpDataExport
{
public:
	static void setExportAddress(const QString &address);
	static void writeMeasurement(const QUuid &devId,const ARpcMessage &m);

private:
	static const quint16 exportPort=6081;
	static QUdpSocket socket;
	static bool enabled;
	static QHostAddress exportAddress;
	static ARpcMessageParser parser;
	static QMutex mutex;
};

#endif // UDPDATAEXPORT_H
