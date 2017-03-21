#include "ARpcContinuousStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include <QDir>
#include <QDateTime>

ARpcContinuousStorage::ARpcContinuousStorage(QObject *parent)
	:ARpcISensorStorage(parent)
{
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}

bool ARpcContinuousStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		int addIndexForTime=0;
		if(val->type()!=ARpcSensor::SINGLE)addIndexForTime=1;
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		if((val2->dims()+addIndexForTime)!=db->noteSizes().size())return false;
		//TODO
		if(addIndexForTime&&db->noteSizes())
		for(int i=0;i<val2->dims();++i)
			if(db->noteSizes()[i]!=sizeof(double))return false;
		if(!db->addBlock())return false;
		for(int i=0;i<val2->dims();++i)
			db->writeNote(i,(const char*)val2->values()[i]);
	}
	else if(val->type()==ARpcSensor::SINGLE)
	{
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		if(val2->dims()!=db->noteSizes().size())return false;
		for(int i=0;i<val2->dims();++i)
			if(db->noteSizes()[i]!=sizeof(double))return false;
		if(!db->addBlock())return false;
		for(int i=0;i<val2->dims();++i)
			db->writeNote(i,(const char*)val2->values()[i]);
	}
}

bool ARpcContinuousStorage::create(const QString &path,const QVector<quint32> &blockNotesSizes)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir(path);
	if(!dir.exists())return false;
	return db->create(dir.absolutePath()+"/data.db",blockNotesSizes);
}

bool ARpcContinuousStorage::openInternal(const QString &path)
{
	return db->open(path+"/data.db");
}
