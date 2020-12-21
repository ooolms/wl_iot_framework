#ifndef USBDRIVER_H
#define USBDRIVER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QTimer>
#include <stdint.h>

struct libusb_device;
struct libusb_device_handle;

class UsbReader
	:public QThread
{
	Q_OBJECT
public:
	explicit UsbReader(libusb_device_handle *h,uint8_t wEp,uint8_t rEp,QObject *parent=0);
	virtual ~UsbReader();
	void writeData(QByteArray data);

signals:
	void newData(QByteArray rData);
	void readError();
	void writeError();

protected:
	virtual void run();

private:
	void writePendingData();
	void onTimer();

private:
	QTimer *t;
	QMutex m;
	libusb_device_handle *devHandle;
	QByteArray rData,wData;
	uint8_t writeEp,readEp;
};

class UsbDriver
	:public QObject
{
	Q_OBJECT
public:
	enum Error
	{
		NoError,
		AccessError,
		ReadError,
		WriteError,
		UnknownError
	};

public:
	explicit UsbDriver(libusb_device *d,uint8_t ifNum,uint8_t devCfg,uint8_t wEp,uint8_t rEp,QObject *parent=0);
	bool open();
	bool isOpened();
	void close();
	Error errorCode();
	QString errorString();
	void write(const QByteArray &data);
	void startReader();

signals:
	void newData(QByteArray data);
	void error();

private:
	libusb_device *dev;
	libusb_device_handle *devHandle;
	Error lastError;
	uint8_t writeEp,readEp,interfaceNum,deviceConfiguratoin;
	UsbReader *reader;
};

#endif // USBDRIVER_H
