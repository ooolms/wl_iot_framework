#include "ARpcPacketSensorValue.h"

ARpcPacketSensorValue::ARpcPacketSensorValue(int dims)
{
	valueType=ARpcSensor::PACKET;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
	timestamp=0;
}

ARpcPacketSensorValue::ARpcPacketSensorValue(int dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::PACKET_LT;
	else valueType=ARpcSensor::PACKET_GT;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
}

ARpcSensor::Type ARpcPacketSensorValue::type()const
{
	return valueType;
}

bool ARpcPacketSensorValue::parse(const ARpcMessage &m)
{
	if(valueType==ARpcSensor::PACKET&&m.args.count()!=1)return false;
	else if((valueType==ARpcSensor::PACKET_LT||valueType==ARpcSensor::PACKET_GT)&&m.args.count()!=2)return false;
	QByteArray rawValues;
	if(valueType!=ARpcSensor::PACKET)
	{
		bool ok=false;
		timestamp=m.args[0].toLongLong(&ok);
		if(!ok)return false;
		rawValues=QByteArray::fromBase64(m.args[1].toUtf8());
	}
	else rawValues=QByteArray::fromBase64(m.args[0].toUtf8());
	if(rawValues.size()%sizeof(float)!=0)return false;
	int rawArrSize=rawValues.size()/sizeof(float);
	if(rawArrSize%dimensions!=0)return false;
	int valuesCount=rawArrSize/dimensions;
	valuesList.clear();
	for(int i=0;i<valuesCount;++i)
	{
		valuesList.append(QVector<double>());
		valuesList.last().resize(dimensions);
		for(int j=0;j<dimensions;++j)
			valuesList.last()[j]=rawValues[i*dimensions+j];
	}
	return true;
}

const QVector<QVector<double>>& ARpcPacketSensorValue::values()const
{
	return valuesList;
}
