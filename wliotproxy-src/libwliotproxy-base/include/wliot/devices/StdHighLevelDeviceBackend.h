#ifndef STDHIGHLEVELDEVICEBACKEND_H
#define STDHIGHLEVELDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "wliot/devices/ILowLevelDeviceBackend.h"
#include "wliot/devices/StreamParser.h"

class StdHighLevelDeviceBackend
	:public IHighLevelDeviceBackend
{
	Q_OBJECT
public:
	explicit StdHighLevelDeviceBackend(ILowLevelDeviceBackend *le,QObject *parent=nullptr);
	virtual bool writeMessageToDevice(const Message &m)override;
	virtual bool isConnected()const override;

private slots:
	void onNewData(const QByteArray &data);

private:
	ILowLevelDeviceBackend *lowLvlBackend;
	StreamParser parser;
};

#endif // STDHIGHLEVELDEVICEBACKEND_H
