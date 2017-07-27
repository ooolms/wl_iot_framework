#include "LsTtyUsbDevices.h"
#include <QDir>
#include <QFile>
#include <QMap>

QString readFileData(const QString &path)
{
	QString data;
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))return data;
	data=QString::fromUtf8(file.readAll());
	data.remove('\n');
	file.close();
	return data;
}

QList<LsTtyUsbDevices::DeviceInfo> LsTtyUsbDevices::allTtyUsbDevices()
{
	QList<DeviceInfo> retVal;
	QDir dir("/sys/bus/usb/devices");
	QMap<QString,DeviceInfo> allDevices;
	QStringList devs=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &usbBusNumber:devs)//enum devices
	{
		if(usbBusNumber.contains(":"))continue;
		QDir devDir(dir.path()+"/"+usbBusNumber);
		DeviceInfo info;
		info.usbBusNumber=usbBusNumber;
		info.productId=readFileData(devDir.absoluteFilePath("idProduct"));
		info.productString=readFileData(devDir.absoluteFilePath("product"));
		info.vendorId=readFileData(devDir.absoluteFilePath("idVendor"));
		info.manufacturerString=readFileData(devDir.absoluteFilePath("manufacturer"));
		allDevices[usbBusNumber]=info;
	}
	for(QString &usbInterfaceNumber:devs)//enum interfaces
	{
		if(!usbInterfaceNumber.contains(":"))continue;
		QDir ifDir(dir.path()+"/"+usbInterfaceNumber);
		if(!ifDir.exists("tty"))continue;
		QStringList ttyPortNameTmp=QDir(ifDir.absoluteFilePath("tty")).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		if(ttyPortNameTmp.isEmpty())continue;
		QString busNum=usbInterfaceNumber.mid(0,usbInterfaceNumber.indexOf(":"));
		if(!allDevices.contains(busNum))continue;
		DeviceInfo info=allDevices[busNum];
		info.interfaceNumber=readFileData(ifDir.filePath("bInterfaceNumber"));
		info.ttyPortName=ttyPortNameTmp[0];
		retVal.append(info);
	}
	return retVal;
}
