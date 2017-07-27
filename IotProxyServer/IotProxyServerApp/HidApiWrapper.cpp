#include "HidApiWrapper.h"
#include "hidapi/hidapi.h"

HidApiWrapper::HidApiWrapper()
{
	hid_init();
}

HidApiWrapper::~HidApiWrapper()
{
	hid_exit();
}

HidApiWrapper &HidApiWrapper::inst()
{
	static HidApiWrapper var;
	return var;
}

void HidApiWrapper::enumerate()
{
	devices.clear();
	hid_device_info *info=hid_enumerate(0,0);
	if(!info)return;
	hid_device_info *i=info;
	while(i)
	{
		HidDeviceInfo s;
		s.interfaceNumber=i->interface_number;
		s.manufacturerString=QString::fromWCharArray(i->manufacturer_string);
		s.path=QString::fromUtf8(i->path);
		s.productId=i->product_id;
		s.productString=QString::fromWCharArray(i->product_string);
		s.releaseNumber=i->release_number;
		s.serialNumber=QString::fromWCharArray(i->serial_number);
		s.vendorId=i->vendor_id;
		devices.append(s);
		i=i->next;
	}
	hid_free_enumeration(info);
}

const QList<HidApiWrapper::HidDeviceInfo> &HidApiWrapper::allDevices()
{
	return devices;
}

bool HidApiWrapper::findDevByPath(const QString &path,HidApiWrapper::HidDeviceInfo &info)
{
	for(auto &i:devices)
		if(i.path==path)
		{
			info=i;
			return true;
		}
	return false;
}
