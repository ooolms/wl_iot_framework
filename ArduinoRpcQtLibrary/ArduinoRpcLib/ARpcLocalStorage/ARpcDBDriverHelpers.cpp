#include "ARpcDBDriverHelpers.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

ARpcDBDriverHelpers::ARpcDBDriverHelpers()
{
	timeRule=ARpcISensorStorage::DONT_TOUCH;
}

ARpcDBDriverHelpers::ARpcDBDriverHelpers(ARpcISensorStorage::TimestampRule rule)
{
	timeRule=rule;
}

QByteArray ARpcDBDriverHelpers::packSensorValue(const ARpcISensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	getTimeFromVal(val,hasTime,timestamp);
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		quint32 dims=val2->dims();
		QByteArray data;
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		data.append((const char*)&dims,sizeof(quint32));
		data.append((const char*)val2->values().data(),dims*sizeof(ARpcSingleSensorValue::ValueType));
		return data;
	}
	if(val->type()==ARpcSensor::PACKET||val->type()==ARpcSensor::PACKET_LT||val->type()==ARpcSensor::PACKET_GT)
	{
		const ARpcPacketSensorValue *val2=(const ARpcPacketSensorValue*)val;
		quint32 dims=val2->dims();
		QByteArray data;
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		data.append((const char *)&dims,sizeof(quint32));
		data.append((const char*)val2->values().data(),
			val2->valuesCount()*val2->dims()*sizeof(ARpcPacketSensorValue::ValueType));
		return data;
	}
	else if(val->type()==ARpcSensor::TEXT)
	{
		const ARpcTextSensorValue *val2=(const ARpcTextSensorValue*)val;
		QByteArray data;
		if(hasTime)data.append((const char*)&timestamp,sizeof(qint64));
		data.append(val2->value().toUtf8());
		return data;
	}
	return QByteArray();
}

ARpcISensorValue* ARpcDBDriverHelpers::unpackSensorValue(ARpcSensor::Type type,const QByteArray &data)
{
	int hasTime=0;
	detectIfHasTime(type,hasTime);
	if(type==ARpcSensor::SINGLE||type==ARpcSensor::SINGLE_LT||type==ARpcSensor::SINGLE_GT)
	{
		if((quint32)data.size()<((hasTime?sizeof(qint64):0)+sizeof(quint32)))return 0;
		quint32 dims=*((const quint32*)(data.constData()+(hasTime?sizeof(qint64):0)));
		const void *dataPointer=(data.constData()+(hasTime?sizeof(qint64):0)+sizeof(quint32));
		quint32 dataSize=data.size()-sizeof(quint32)-(hasTime?sizeof(qint64):0);
		if(dataSize%sizeof(ARpcSingleSensorValue::ValueType)!=0||
			(dataSize/sizeof(ARpcSingleSensorValue::ValueType))!=dims)return 0;
		ARpcSingleSensorValue *retVal=0;
		if(type==ARpcSensor::SINGLE)retVal=new ARpcSingleSensorValue(dims);
		else if(type==ARpcSensor::SINGLE_LT)retVal=new ARpcSingleSensorValue(dims,true);
		else retVal=new ARpcSingleSensorValue(dims,false);
		if(hasTime)
		{
			qint64 t=*((const qint64*)data.constData());
			retVal->setTime(t);
		}
		retVal->fromData((const ARpcSingleSensorValue::ValueType*)dataPointer,dims);
		return retVal;
	}
	if(type==ARpcSensor::PACKET||type==ARpcSensor::PACKET_LT||type==ARpcSensor::PACKET_GT)
	{
		if((quint32)data.size()<((hasTime?sizeof(qint64):0)+sizeof(quint32)))return 0;
		quint32 dims=*((const quint32*)(data.constData()+(hasTime?sizeof(qint64):0)));
		const void *dataPointer=(data.constData()+(hasTime?sizeof(qint64):0)+sizeof(quint32));
		quint32 dataSize=data.size()-sizeof(quint32)-(hasTime?sizeof(qint64):0);
		if(dataSize%sizeof(ARpcPacketSensorValue::ValueType)!=0||
			(dataSize/sizeof(ARpcPacketSensorValue::ValueType))%dims!=0)return 0;
		quint32 count=dataSize/sizeof(ARpcPacketSensorValue::ValueType)/dims;
		ARpcPacketSensorValue *retVal=0;
		if(type==ARpcSensor::PACKET)retVal=new ARpcPacketSensorValue(dims);
		else if(type==ARpcSensor::PACKET_LT)retVal=new ARpcPacketSensorValue(dims,true);
		else retVal=new ARpcPacketSensorValue(dims,false);
		if(hasTime)
		{
			qint64 t=*((const qint64*)data.constData());
			retVal->setTime(t);
		}
		retVal->fromData((const ARpcPacketSensorValue::ValueType*)dataPointer,dims,count);
		return retVal;
	}
	else if(type==ARpcSensor::TEXT)
	{
		if((quint32)data.size()<sizeof(qint64))return 0;
		qint64 t=*((const qint64*)data.constData());
		ARpcTextSensorValue *retVal=new ARpcTextSensorValue;
		retVal->setTime(t);
		retVal->fromData(data.constData()+sizeof(qint64),data.size()-sizeof(qint64));
		return retVal;
	}
	return 0;
}

void ARpcDBDriverHelpers::detectIfHasTime(ARpcSensor::Type type,int &hasTime)
{
	if(timeRule==ARpcISensorStorage::ADD_GT||type==ARpcSensor::TEXT)
		hasTime=1;
	else if(timeRule==ARpcISensorStorage::DROP_TIME||(type==ARpcSensor::SINGLE||type==ARpcSensor::PACKET))
		hasTime=0;
	else hasTime=1;
}

void ARpcDBDriverHelpers::getTimeFromVal(const ARpcISensorValue *val,int &hasTime,qint64 &timestamp)
{
	hasTime=0;
	timestamp=0;
	if(val->type()==ARpcSensor::TEXT)
	{
		hasTime=1;
		timestamp=val->time();
	}
	else if(timeRule==ARpcISensorStorage::DROP_TIME){}
	else if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::PACKET)
	{
		if(timeRule==ARpcISensorStorage::ADD_GT)
		{
			hasTime=1;
			timestamp=QDateTime::currentMSecsSinceEpoch();
		}
	}
	else if((val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::PACKET_LT)&&
		timeRule==ARpcISensorStorage::ADD_GT)
	{
		hasTime=1;
		timestamp=QDateTime::currentMSecsSinceEpoch();
	}
	else
	{
		hasTime=1;
		timestamp=val->time();
	}
}

QVector<quint32> ARpcDBDriverHelpers::sizesForFixedBlocksDb(const ARpcISensorValue &templateValue)
{
	QVector<quint32> retVal;
	int hasTime;
	detectIfHasTime(templateValue.type(),hasTime);
	if(templateValue.type()==ARpcSensor::SINGLE||templateValue.type()==ARpcSensor::SINGLE_LT||
		templateValue.type()==ARpcSensor::SINGLE_GT)
	{
		const ARpcSingleSensorValue &val=(const ARpcSingleSensorValue&)templateValue;
		if(hasTime)retVal.append(sizeof(qint64));
		retVal.append(sizeof(quint32));
		for(quint32 i=0;i<val.dims();++i)
			retVal.append(sizeof(ARpcSingleSensorValue::ValueType));
	}
	else if(templateValue.type()==ARpcSensor::PACKET||templateValue.type()==ARpcSensor::PACKET_LT||
		templateValue.type()==ARpcSensor::PACKET_GT)
	{
		const ARpcPacketSensorValue &val=(const ARpcPacketSensorValue&)templateValue;
		if(hasTime)retVal.append(sizeof(quint64));
		retVal.append(sizeof(quint32));
		for(quint32 i=0;i<val.dims()*val.valuesCount();++i)
			retVal.append(sizeof(ARpcPacketSensorValue::ValueType));
	}
	else if(templateValue.type()==ARpcSensor::TEXT)
	{
		const ARpcTextSensorValue &val=(const ARpcTextSensorValue&)templateValue;
		if(hasTime)retVal.append(sizeof(quint64));
		retVal.append(val.value().toUtf8().size());
	}
	return retVal;
}
