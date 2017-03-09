#ifndef ARPCDEVICESTATE_H
#define ARPCDEVICESTATE_H

#include <QMap>

class ARpcDeviceState
{
public:
	QMap<QString,QMap<int,QString>> commandParams;
	QMap<QString,QString> additionalAttributes;
};

#endif // ARPCDEVICESTATE_H
