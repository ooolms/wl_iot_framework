#ifndef ARPCSTORAGEID_H
#define ARPCSTORAGEID_H

#include <QUuid>
#include <QMetaType>

class ARpcStorageId
{
public:
	bool operator==(const ARpcStorageId &id)const
	{
		return deviceId==id.deviceId&&sensorName==id.sensorName;
	}

	bool operator<(const ARpcStorageId &id)const
	{
		if(deviceId==id.deviceId)
			return sensorName<id.sensorName;
		return deviceId<id.deviceId;
	}

public:
	QUuid deviceId;
	QByteArray sensorName;
};

Q_DECLARE_METATYPE(ARpcStorageId)
uint qHash(const ARpcStorageId &id,uint seed=0);

#endif // ARPCSTORAGEID_H
