#ifndef IOTSERVERVIRTUALDEVICECALLBACK_H
#define IOTSERVERVIRTUALDEVICECALLBACK_H

#include <QObject>

class IotServerVirtualDeviceClient;

class IotServerVirtualDeviceCallback
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerVirtualDeviceCallback(IotServerVirtualDeviceClient *dev,QObject *parent=nullptr);
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)=0;

protected:
	void commandParamStateChanged(const QByteArray &cmd,int paramIndex,const QByteArray &value);
	void additionalStateChanged(const QByteArray &paramName,const QByteArray &value);

private:
	IotServerVirtualDeviceClient *mDev;
};

#endif // IOTSERVERVIRTUALDEVICECALLBACK_H
