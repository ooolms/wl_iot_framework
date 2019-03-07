/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
		bool isTtyDevice=false;
		QStringList ttyPortNameTmp;
		if(ifDir.exists("tty"))
		{
			ttyPortNameTmp=QDir(ifDir.absoluteFilePath("tty")).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
			if(!ttyPortNameTmp.isEmpty())isTtyDevice=true;
		}
		else
		{
			QStringList subdirs=ifDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
			for(auto &sDir:subdirs)
			{
				if(sDir.startsWith("tty"))
				{
					ttyPortNameTmp.append(sDir);
					isTtyDevice=true;
					break;
				}
			}
		}
		if(!isTtyDevice)continue;
		QString busNum=usbInterfaceNumber.mid(0,usbInterfaceNumber.indexOf(":"));
		if(!allDevices.contains(busNum))continue;
		DeviceInfo info=allDevices[busNum];
		info.interfaceNumber=readFileData(ifDir.filePath("bInterfaceNumber"));
		info.ttyPortName=ttyPortNameTmp[0];
		retVal.append(info);
	}
	return retVal;
}
