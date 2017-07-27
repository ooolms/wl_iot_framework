#ifndef HIDAPIWRAPPER_H
#define HIDAPIWRAPPER_H

#include <QString>
#include <QList>

class HidApiWrapper
{
public:
	struct HidDeviceInfo // Qt copy of hid_device_info from hidapi library
	{
		QString path;
		unsigned short vendorId;
		unsigned short productId;
		QString serialNumber;
		unsigned short releaseNumber;
		QString manufacturerString;
		QString productString;
		int interfaceNumber;
	};

private:
	HidApiWrapper();
	~HidApiWrapper();
	HidApiWrapper(const HidApiWrapper &);
	HidApiWrapper& operator=(const HidApiWrapper &);

public:
	static HidApiWrapper& inst();
	void enumerate();
	const QList<HidDeviceInfo>& allDevices();
	bool findDevByPath(const QString &path,HidDeviceInfo &info);

private:
	QList<HidDeviceInfo> devices;
};

#endif // HIDAPIWRAPPER_H
