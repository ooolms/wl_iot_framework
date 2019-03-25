#include "UsbDriver.h"
#include <QEventLoop>
#include <libusb.h>
#include <QDebug>

UsbReader::UsbReader(libusb_device_handle *h,uint8_t wEp,uint8_t rEp,QObject *parent)
	:QThread(parent)
{
	t=0;
	devHandle=h;
	writeEp=wEp;
	readEp=rEp;
}

UsbReader::~UsbReader()
{
	terminate();
	wait();
}

void UsbReader::writeData(QByteArray data)
{
	if(!devHandle)return;
	int written=0;
	libusb_bulk_transfer(devHandle,writeEp,(unsigned char*)data.data(),data.size(),&written,0);
	if(written!=data.size())
		emit writeError();
}

void UsbReader::run()
{
	if(t)delete t;
	t=new QTimer;
	t->setInterval(100);
	t->setSingleShot(true);
	connect(t,&QTimer::timeout,this,&UsbReader::onTimer);

	QByteArray dd;
	dd.resize(1024);
	QEventLoop loop;
	while(!isInterruptionRequested())
	{
		int readSz=0;
		int r=libusb_bulk_transfer(devHandle,readEp,(unsigned char*)dd.data(),dd.size(),&readSz,100);
		if(r!=0&&r!=LIBUSB_ERROR_TIMEOUT)
		{
			if(!isInterruptionRequested())
				emit readError();
		}
		if(readSz!=0)
		{
			QMutexLocker l(&m);
			rData.append(dd.constData(),readSz);
			if(!t->isActive())
				t->start();
		}
		loop.processEvents();
	}
	delete t;
	t=0;
}

void UsbReader::onTimer()
{
	QMutexLocker l(&m);
//	qDebug()<<"RAW SERIAL READ: "<<rData;
	if(!rData.isEmpty())
	{
		QByteArray d;
		std::swap(d,rData);
		emit newData(std::move(d));
	}
}

UsbDriver::UsbDriver(libusb_device *d,uint8_t ifNum,uint8_t devCfg,uint8_t wEp,uint8_t rEp,QObject *parent)
	:QObject(parent)
{
	interfaceNum=ifNum;
	writeEp=wEp;
	readEp=rEp;
	deviceConfiguratoin=devCfg;
}

bool UsbDriver::open()
{
	if(devHandle)return false;
	if(readEp&0x80||!writeEp&0x80)
	{
		lastError=AccessError;
		return false;
	}
	if(libusb_open(dev,&devHandle))
	{
		lastError=AccessError;
		return false;
	}
	if(libusb_kernel_driver_active(devHandle,interfaceNum))
		libusb_detach_kernel_driver(devHandle,interfaceNum);
	if(libusb_set_configuration(devHandle,deviceConfiguratoin)<0)
	{
		qDebug()<<"Device configuration error";
		libusb_attach_kernel_driver(devHandle,interfaceNum);
		libusb_close(devHandle);
		devHandle=0;
		lastError=AccessError;
		return 0;
	}
	if(libusb_claim_interface(devHandle,interfaceNum)<0)
	{
		qDebug()<<"Interface error";
		libusb_attach_kernel_driver(devHandle,interfaceNum);
		libusb_close(devHandle);
		devHandle=0;
		lastError=AccessError;
		return false;
	}
	reader=new UsbReader(devHandle,writeEp,readEp,this);
	connect(reader,&UsbReader::newData,this,&UsbDriver::newData,Qt::QueuedConnection);
	return true;
}

bool UsbDriver::isOpened()
{
	return devHandle!=0;
}

void UsbDriver::close()
{
	reader->requestInterruption();
	libusb_attach_kernel_driver(devHandle,interfaceNum);
	libusb_close(devHandle);
	devHandle=0;
	reader->wait(1000);
	delete reader;
}

UsbDriver::Error UsbDriver::errorCode()
{
	return lastError;
}

QString UsbDriver::errorString()
{
	if(lastError==AccessError)
		return "Access error";
	else if(lastError==ReadError)
		return "Read error";
	else if(lastError==WriteError)
		return "Write error";
	else if(lastError==UnknownError)
		return "Unknown error";
	return QString();
}

void UsbDriver::write(const QByteArray &data)
{
	reader->writeData(data);
}

void UsbDriver::startReader()
{
	reader->start();
}
