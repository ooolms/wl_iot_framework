#include "ARpcPacketSensorValue.h"

ARpcPacketSensorValue::ARpcPacketSensorValue(quint32 dims)
{
	valueType=ARpcSensor::PACKET;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	timestamp=0;
	valCount=0;
}

ARpcPacketSensorValue::ARpcPacketSensorValue(quint32 dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::PACKET_LT;
	else valueType=ARpcSensor::PACKET_GT;
	dimensions=dims;
	if(dimensions==0)dimensions=1;
	valCount=0;
}

ARpcSensor::Type ARpcPacketSensorValue::type()const
{
	return valueType;
}

bool ARpcPacketSensorValue::parse(ARpcMessage m)
{
	if(m.args.isEmpty())return false;
	m.args.removeFirst();
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
	int numbersCount=rawValues.size()/sizeof(float);
	if(numbersCount%dimensions!=0)return false;
	valCount=numbersCount/dimensions;
	valuesList.resize(numbersCount);
	memcpy(valuesList.data(),rawValues.constData(),rawValues.size());
	return true;
}

const QVector<float>& ARpcPacketSensorValue::values()const
{
	return valuesList;
}

float ARpcPacketSensorValue::at(quint32 valIndex,quint32 dimension)const
{
	return valuesList[valIndex*dimensions+dimension];
}

qint64 ARpcPacketSensorValue::time()const
{
	return timestamp;
}

void ARpcPacketSensorValue::setTime(qint64 t)
{
	timestamp=t;
}

void ARpcPacketSensorValue::fromData(const float *vals,quint32 dims,quint32 count)
{
	dimensions=dims;
	valCount=count;
	valuesList.resize(dimensions*valCount);
	memcpy(valuesList.data(),vals,valCount*dimensions*sizeof(float));
}

quint32 ARpcPacketSensorValue::dims()const
{
	return dimensions;
}

quint32 ARpcPacketSensorValue::valuesCount()const
{
	return valCount;
}
