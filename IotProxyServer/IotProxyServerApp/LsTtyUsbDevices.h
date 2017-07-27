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
	static QList<DeviceInfo> allTtyUsbDevices();
};

#endif // LSTTYUSBDEVICES_H
