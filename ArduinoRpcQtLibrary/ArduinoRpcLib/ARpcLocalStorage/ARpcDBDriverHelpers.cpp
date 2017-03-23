#include "ARpcDBDriverHelpers.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

//CRIT read/write SINGLE values to chained db

bool ARpcDBDriverHelpers::writeSensorValueToFixedBlocksDB(
	ARpcDBDriverFixedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime)
{
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		int hasTime=0;
		if(val->type()!=ARpcSensor::SINGLE)hasTime=1;
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		if((val2->dims()+hasTime)!=(quint32)db->noteSizes().size())return false;
		if(hasTime&&db->noteSizes()[0]!=sizeof(qint64))return false;
		for(quint32 i=0;i<val2->dims();++i)
			if(db->noteSizes()[i+hasTime]!=sizeof(double))return false;
		if(!db->addBlock())return false;
		if(hasTime)
		{
			qint64 t;
			if(val->type()==ARpcSensor::SINGLE_LT&&replaceLocalTimeWithGlobalTime)
				t=QDateTime::currentMSecsSinceEpoch();
			else t=val2->time();
			db->writeNote(0,&t);
		}
		for(quint32 i=0;i<val2->dims();++i)
			db->writeNote(i+hasTime,&val2->values()[i]);
		return true;
	}
	else return false;
}

bool ARpcDBDriverHelpers::writeSensorValueToChainedBlocksDB(
	ARpcDBDriverChainedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime)
{
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		int hasTime=0;
		if(val->type()!=ARpcSensor::SINGLE)hasTime=1;
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		QByteArray data;
		if(hasTime)
		{
			qint64 t;
			if(val->type()==ARpcSensor::SINGLE_LT&&replaceLocalTimeWithGlobalTime)
				t=QDateTime::currentMSecsSinceEpoch();
			else t=val2->time();
			data.append((const char*)&t,sizeof(qint64));
		}
		quint32 dims=val2->dims();
		data.append((const char*)&dims,sizeof(quint32));
		data.append((const char*)val2->values().data(),dims*sizeof(double));
		return db->writeBlock(data);
	}
	if(val->type()==ARpcSensor::PACKET||val->type()==ARpcSensor::PACKET_LT||val->type()==ARpcSensor::PACKET_GT)
	{
		const ARpcPacketSensorValue *val2=(const ARpcPacketSensorValue*)val;
		QByteArray data;
		if(val->type()!=ARpcSensor::PACKET)
		{
			qint64 t;
			if(val->type()==ARpcSensor::PACKET_LT&&replaceLocalTimeWithGlobalTime)
				t=QDateTime::currentMSecsSinceEpoch();
			else t=val2->time();
			data.append((const char*)&t,sizeof(qint64));
		}
		quint32 dims=val2->dims();
		data.append((const char *)&dims,sizeof(quint32));
		data.append((const char*)val2->values().data(),val2->valuesCount()*val2->dims()*sizeof(float));
		return db->writeBlock(data);
	}
	else if(val->type()==ARpcSensor::TEXT)
	{
		const ARpcTextSensorValue *val2=(const ARpcTextSensorValue*)val;
		QByteArray data;
		qint64 t=val2->time();
		data.append((const char*)&t,sizeof(qint64));
		data.append(val2->value().toUtf8());
		return db->writeBlock(data);
	}
	else return false;
}

ARpcISensorValue *ARpcDBDriverHelpers::readSensorValueFromFixedBlocksDB(
	ARpcDBDriverFixedBlocks *db,ARpcSensor::Type type,quint64 blockIndex)
{
	if(type==ARpcSensor::SINGLE||type==ARpcSensor::SINGLE_LT||type==ARpcSensor::SINGLE_GT)
	{
		int hasTime=0;
		if(type!=ARpcSensor::SINGLE)hasTime=1;
		if(hasTime&&db->noteSizes()[0]!=sizeof(qint64))return 0;
		for(int i=hasTime;i<db->noteSizes().size();++i)
			if(db->noteSizes()[i]!=sizeof(double))return 0;
		int dims=db->noteSizes().size()-hasTime;
		QByteArray data;
		if(!db->readBlock(blockIndex,data))return 0;
		ARpcSingleSensorValue *retVal=0;
		if(type==ARpcSensor::SINGLE)retVal=new ARpcSingleSensorValue(dims);
		else if(type==ARpcSensor::SINGLE_LT)retVal=new ARpcSingleSensorValue(dims,true);
		else retVal=new ARpcSingleSensorValue(dims,false);
		if(hasTime)
		{
			qint64 t=*((qint64*)data.constData());
			retVal->setTime(t);
		}
		retVal->fromData((const double*)(data.constData()+sizeof(qint64)*hasTime),dims);
		return retVal;
	}
	else return 0;
}

ARpcISensorValue *ARpcDBDriverHelpers::readSensorValueFromChainedBlocksDB(
	ARpcDBDriverChainedBlocks *db,ARpcSensor::Type type,quint32 blockIndex)
{
	if(type==ARpcSensor::SINGLE||type==ARpcSensor::SINGLE_LT||type==ARpcSensor::SINGLE_GT)
	{
		int hasTime=0;
		if(type!=ARpcSensor::SINGLE)hasTime=1;
		QByteArray data;
		if(!db->readBlock(blockIndex,data))return 0;
		if((quint32)data.size()<((hasTime?sizeof(qint64):0)+sizeof(quint32)))return 0;
		quint32 dims=*((const quint32*)(data.constData()+(hasTime?sizeof(qint64):0)));
		const void *dataPointer=(data.constData()+(hasTime?sizeof(qint64):0)+sizeof(quint32));
		quint32 dataSize=data.size()-sizeof(quint32)-(hasTime?sizeof(qint64):0);
		if(dataSize%sizeof(double)!=0||(dataSize/sizeof(double))!=dims)return 0;
		ARpcSingleSensorValue *retVal=0;
		if(type==ARpcSensor::SINGLE)retVal=new ARpcSingleSensorValue(dims);
		else if(type==ARpcSensor::SINGLE_LT)retVal=new ARpcSingleSensorValue(dims,true);
		else retVal=new ARpcSingleSensorValue(dims,false);
		if(hasTime)
		{
			qint64 t=*((const qint64*)data.constData());
			retVal->setTime(t);
		}
		retVal->fromData((const double*)dataPointer,dims);
		return retVal;
	}
	if(type==ARpcSensor::PACKET||type==ARpcSensor::PACKET_LT||type==ARpcSensor::PACKET_GT)
	{
		int hasTime=0;
		if(type!=ARpcSensor::PACKET)hasTime=1;
		QByteArray data;
		if(!db->readBlock(blockIndex,data))return 0;
		if((quint32)data.size()<((hasTime?sizeof(qint64):0)+sizeof(quint32)))return 0;
		quint32 dims=*((const quint32*)(data.constData()+(hasTime?sizeof(qint64):0)));
		const void *dataPointer=(data.constData()+(hasTime?sizeof(qint64):0)+sizeof(quint32));
		quint32 dataSize=data.size()-sizeof(quint32)-(hasTime?sizeof(qint64):0);
		if(dataSize%sizeof(float)!=0||(dataSize/sizeof(float))%dims!=0)return 0;
		quint32 count=dataSize/sizeof(float)/dims;
		ARpcPacketSensorValue *retVal=0;
		if(type==ARpcSensor::PACKET)retVal=new ARpcPacketSensorValue(dims);
		else if(type==ARpcSensor::PACKET_LT)retVal=new ARpcPacketSensorValue(dims,true);
		else retVal=new ARpcPacketSensorValue(dims,false);
		if(hasTime)
		{
			qint64 t=*((const qint64*)data.constData());
			retVal->setTime(t);
		}
		retVal->fromData((const float*)dataPointer,dims,count);
		return retVal;
	}
	else if(type==ARpcSensor::TEXT)
	{
		QByteArray data;
		if(!db->readBlock(blockIndex,data))return 0;
		if((quint32)data.size()<sizeof(qint64))return 0;
		qint64 t=*((const qint64*)data.constData());
		ARpcTextSensorValue *retVal=new ARpcTextSensorValue;
		retVal->setTime(t);
		retVal->fromData(data.constData()+sizeof(qint64),data.size()-sizeof(qint64));
		return retVal;
	}
	else return 0;
}
