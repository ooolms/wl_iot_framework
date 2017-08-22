#include "UdpDataExport.h"
#include <QUuid>

QUdpSocket UdpDataExport::socket;
QHostAddress UdpDataExport::exportAddress;
bool UdpDataExport::enabled=false;
ARpcMessageParser UdpDataExport::parser;
QMutex UdpDataExport::mutex;

void UdpDataExport::setExportAddress(const QString &address)
{
	enabled=!address.isEmpty();
	exportAddress=QHostAddress(address);
}

void UdpDataExport::writeMeasurement(const QUuid &devId,const ARpcMessage &m)
{
	if(!enabled)return;
	QMutexLocker lock(&mutex);
	ARpcMessage msg(devId.toString(),m.args);
	socket.writeDatagram((parser.dump(msg)+ARpcConfig::msgDelim).toUtf8(),exportAddress,exportPort);
}

