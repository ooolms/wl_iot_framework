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

#ifndef LSTTYUSBDEVICES_H
#define LSTTYUSBDEVICES_H

#include <QList>
#include <QString>

class LsTtyUsbDevices
{
public:
	struct DeviceInfo
	{
		DeviceInfo()
		{
			vendorId="0";
			productId="0";
			interfaceNumber="0";
		}

		QString ttyPortName;
		QString usbBusNumber;//dir name from /sys/bus/usb/devices
		QString vendorId;
		QString productId;
//		QString serialNumber;
//		unsigned short releaseNumber;
		QString manufacturerString;
		QString productString;
		QString interfaceNumber;
	};

public:
	static QMap<QString,DeviceInfo> allTtyUsbDevices();
};

#endif // LSTTYUSBDEVICES_H
