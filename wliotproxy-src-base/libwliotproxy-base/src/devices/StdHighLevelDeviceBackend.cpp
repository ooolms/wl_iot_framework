#include "wliot/devices/StdHighLevelDeviceBackend.h"
#include <QDebug>

using namespace WLIOT;

StdHighLevelDeviceBackend::StdHighLevelDeviceBackend(ILowLevelDeviceBackend *le,QObject *parent)
	:IHighLevelDeviceBackend(parent)
{
	lowLvlBackend=le;
	lowLvlBackend->setParent(this);
	connect(lowLvlBackend,SIGNAL(newData(QByteArray)),this,SLOT(onNewData(QByteArray)));
	connect(lowLvlBackend,&ILowLevelDeviceBackend::connected,this,&StdHighLevelDeviceBackend::connected);
	connect(lowLvlBackend,&ILowLevelDeviceBackend::disconnected,this,&StdHighLevelDeviceBackend::disconnected);
	connect(&parser,&StreamParser::streamWasReset,this,&StdHighLevelDeviceBackend::deviceReset);
	connect(&parser,SIGNAL(newMessage(WLIOT::Message)),this,SIGNAL(newMessageFromDevice(WLIOT::Message)));
}

StdHighLevelDeviceBackend::~StdHighLevelDeviceBackend()
{
	emit destroyedBeforeQObject();
}

bool StdHighLevelDeviceBackend::writeMessageToDevice(const Message &m)
{
	return lowLvlBackend->writeData(m.dump());
}

bool StdHighLevelDeviceBackend::isConnected()const
{
	return lowLvlBackend->isConnected();
}

void StdHighLevelDeviceBackend::forceDisconnect()
{
	lowLvlBackend->forceDisconnect();
}

QByteArray StdHighLevelDeviceBackend::backendType()const
{
	return lowLvlBackend->backendType();
}

ILowLevelDeviceBackend* StdHighLevelDeviceBackend::backend()
{
	return lowLvlBackend;
}

QString StdHighLevelDeviceBackend::hwAddress()const
{
	return lowLvlBackend->hwAddress();
}

void StdHighLevelDeviceBackend::onNewData(const QByteArray &data)
{
	parser.pushData(data);
}
