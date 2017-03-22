#include "ARpcPacketSensorValue.h"

ARpcPacketSensorValue::ARpcPacketSensorValue(int dims)
{
	valueType=ARpcSensor::PACKET;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
	timestamp=0;
	valCount=0;
	valuesList=0;
}

ARpcPacketSensorValue::ARpcPacketSensorValue(int dims,bool localTimeStamp)
{
	if(localTimeStamp)valueType=ARpcSensor::PACKET_LT;
	else valueType=ARpcSensor::PACKET_GT;
	dimensions=dims;
	if(dimensions<=0)dimensions=1;
	valCount=0;
	valuesList=0;
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
	int rawArrSize=rawValues.size()/sizeof(float);
	if(rawArrSize%dimensions!=0)return false;
	valCount=rawArrSize/dimensions;
	if(valuesList)delete[] valuesList;
	valuesList=new float[rawArrSize];
	memcpy(valuesList,rawValues.constData(),rawValues.size());
	return true;
}

const float* ARpcPacketSensorValue::values()const
{
	return valuesList;
}

float ARpcPacketSensorValue::at(int valIndex,int dimension)const
{
	return valuesList[valIndex*dimensions+dimension];
}

qint64 ARpcPacketSensorValue::time()const
{
	return timestamp;
}

int ARpcPacketSensorValue::dims()const
{
	return dimensions;
}

int ARpcPacketSensorValue::valuesCount()const
{
	return valCount;
}
