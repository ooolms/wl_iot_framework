#include "wliot/devices/UsbDevice.h"
#include "UsbDriver.h"

UsbDevice::UsbDevice(libusb_device *d,uint8_t ifNum,uint8_t devCfg,uint8_t wEp,uint8_t rEp,QObject *parent)
	:RealDevice(parent)
{
	usbDev=new UsbDriver(d,ifNum,devCfg,wEp,rEp,this);

	connect(usbDev,&UsbDriver::newData,this,&UsbDevice::onNewData,Qt::QueuedConnection);
	connect(usbDev,&UsbDriver::error,this,&UsbDevice::onError);
	connect(&streamParser,&StreamParser::newMessage,this,&UsbDevice::onNewMessage);
}

UsbDevice::~UsbDevice()
{
	closeUsb();
}

bool UsbDevice::writeMsgToDevice(const Message &m)
{
	if(!isConnected())
		return false;
	QByteArray data=m.dump();
	usbDev->write(data);
	return true;
}

void UsbDevice::closeUsb()
{
	usbDev->close();
}

void UsbDevice::tryOpen()
{
	if(isConnected())
		return;
	if(!usbDev->open())
		return;
	usbDev->startReader();
	onConnected();
	identify();
}

void UsbDevice::syncFailed()
{
	closeUsb();
}

void UsbDevice::onNewData(const QByteArray &data)
{
	if(!data.isEmpty())
		streamParser.pushData(data);
}

void UsbDevice::onError()
{
	closeUsb();
}
