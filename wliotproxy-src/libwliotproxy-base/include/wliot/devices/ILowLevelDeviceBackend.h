#ifndef ILOWLEVELDEVICEBACKEND_H
#define ILOWLEVELDEVICEBACKEND_H

#include <QObject>

class ILowLevelDeviceBackend
	:public QObject
{
	Q_OBJECT
public:
	explicit ILowLevelDeviceBackend(QObject *parent=nullptr);
	virtual ~ILowLevelDeviceBackend(){}
	virtual bool writeData(const QByteArray &data)=0;
	virtual bool flush()=0;
	virtual bool isConnected()const=0;
	virtual void forceDisconnect()=0;
	virtual QByteArray type()const=0;
	virtual QByteArray portOrAddress()const=0;

signals:
	void connected();
	void disconnected();
	void newData(const QByteArray &data);
};

#endif // ILOWLEVELDEVICEBACKEND_H
