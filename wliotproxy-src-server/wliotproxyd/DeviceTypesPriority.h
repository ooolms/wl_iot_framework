#ifndef DEVICETYPESPRIORITY_H
#define DEVICETYPESPRIORITY_H

#include <QByteArray>
#include <QMap>

class DeviceTypesPriority
{
public:
	static DeviceTypesPriority& inst();
	bool shouldReplace(const QByteArray &existingBackendType,const QByteArray &replacingBackendType);

private:
	DeviceTypesPriority();

private:
	QMap<QByteArray,int> typePriorities;
};

#endif // DEVICETYPESPRIORITY_H
