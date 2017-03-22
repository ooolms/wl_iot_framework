#include "ARpcDBDriverHelpers.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDateTime>

bool ARpcDBDriverHelpers::writeSensorValueToFixedBlocksDB(
	ARpcDBDriverFixedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime)
{
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		int hasTime=0;
		if(val->type()!=ARpcSensor::SINGLE)hasTime=1;
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		if((val2->dims()+hasTime)!=db->noteSizes().size())return false;
		if(hasTime&&db->noteSizes()[0]!=sizeof(qint64))return false;
		for(int i=0;i<val2->dims();++i)
			if(db->noteSizes()[i+hasTime]!=sizeof(double))return false;
		if(!db->addBlock())return false;
		if(hasTime)
		{
			qint64 t;
			if(val->type()==ARpcSensor::SINGLE_LT&&replaceLocalTimeWithGlobalTime)
				t=QDateTime::currentMSecsSinceEpoch();
			else t=val2->time();
			db->writeNote(0,(const char*)&t);
		}
		for(int i=0;i<val2->dims();++i)
			db->writeNote(i+hasTime,(const char*)&val2->values()[i]);
		return true;
	}
	else return false;
}

bool ARpcDBDriverHelpers::writeSensorValueToChainedBlocksDB(
	ARpcDBDriverChainedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime)
{
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
		data.append((const char*)val2->values(),val2->valuesCount()*val2->dims()*sizeof(float));
		return db->writeBlock(data);
	}
	else if(val->type()==ARpcSensor::TEXT)
	{
		const ARpcTextSensorValue *val2=(const ARpcTextSensorValue*)val;
		QByteArray data;
		qint64 t=QDateTime::currentMSecsSinceEpoch();
		data.append((const char*)&t,sizeof(qint64));
		data.append(val2->value().toUtf8());
		return db->writeBlock(data);
	}
	else return false;
}
