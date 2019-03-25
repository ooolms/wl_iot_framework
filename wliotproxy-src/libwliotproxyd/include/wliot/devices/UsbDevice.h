#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <QObject>
#include <stdint.h>
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/StreamParser.h"

class UsbDriver;

struct libusb_device;

//TODO test
class UsbDevice
	:public RealDevice
{
	Q_OBJECT
public:
	explicit UsbDevice(libusb_device *d,uint8_t ifNum,uint8_t devCfg,uint8_t wEp,uint8_t rEp,QObject *parent=nullptr);
	virtual ~UsbDevice();
	virtual bool writeMsgToDevice(const Message &m)override;
	void closeUsb();

public slots:
	void tryOpen();

protected:
	virtual void syncFailed();

private slots:
	void onNewData(const QByteArray &data);
	void onError();
	//TODO onDevDetached

private:
	StreamParser streamParser;
	UsbDriver *usbDev;
};

#endif // USBDEVICE_H
