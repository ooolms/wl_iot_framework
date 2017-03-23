#include "ARpcLastNValuesStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include <QDateTime>

ARpcLastNValuesStorage::ARpcLastNValuesStorage(QObject *parent)
	:ARpcISensorStorage(parent)
{
	storedCount=0;
	opened=false;
}

bool ARpcLastNValuesStorage::create(quint32 storedValuesCount)
{
	if(opened)return false;
	if(storedValuesCount==0)storedValuesCount=1;
	QFile file(dbDir.absolutePath()+"/db.index");
	if(file.exists())return false;
	storedCount=storedValuesCount;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("storedCount",QString::number(storedCount));
	settings.sync();
	if(settings.status()!=QSettings::NoError)return false;
	if(!file.open(QIODevice::WriteOnly))return false;
	file.write("0");
	file.close();
	opened=true;
	startIndex=0;
	return true;
}

ARpcISensorStorage::StoreMode ARpcLastNValuesStorage::getStoreMode()const
{
	return ARpcISensorStorage::LAST_N_VALUES;
}

bool ARpcLastNValuesStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)return false;
	QByteArray data=valToData(val);
	if(data.isEmpty())return false;
	QFile dataFile(dbDir.absolutePath()+"/"+QString::number(startIndex)+".data");
	QFile indexFile(dbDir.absolutePath()+"/db.index");
	if(!dataFile.open(QIODevice::WriteOnly)||!indexFile.open(QIODevice::WriteOnly))return false;
	indexFile.write(QByteArray::number((startIndex+1)%storedCount));
	indexFile.close();
	startIndex=(startIndex+1)%storedCount;
	if(dataFile.write(data)!=data.size())
	{
		dataFile.close();
		dataFile.remove();
	}
	else dataFile.close();
	return true;
}

ARpcISensorValue *ARpcLastNValuesStorage::readValue(quint32 index)
{
	if(index>=storedCount)return 0;
	QFile file(dbDir.absolutePath()+"/"+QString::number((startIndex+index)%storedCount)+".data");
	if(!file.open(QIODevice::ReadOnly))return 0;
	QByteArray data=file.readAll();
	file.close();
	if(data.isEmpty())return 0;
	return valFromData(data);
}

quint32 ARpcLastNValuesStorage::valuesCount()
{
	return storedCount;
}

bool ARpcLastNValuesStorage::openInternal()
{
	if(opened)return false;
	QFile file(dbDir.absolutePath()+"/db.index");
	if(!file.exists())return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	bool ok=false;
	storedCount=settings.value("storedCount").toUInt(&ok);
	if(!ok)return false;
	if(storedCount==0)storedCount=1;
	if(!file.open(QIODevice::ReadOnly))return false;
	startIndex=file.readAll().toULong(&ok);
	file.close();
	if(!ok)return false;
	opened=true;
	return true;
}

void ARpcLastNValuesStorage::closeInternal()
{
	opened=false;
	storedCount=0;
	startIndex=0;
}

QByteArray ARpcLastNValuesStorage::valToData(const ARpcISensorValue *val)
{
	if(val->type()!=valueType)return QByteArray();
	QByteArray retVal;
	if(val->type()==ARpcSensor::SINGLE||val->type()==ARpcSensor::SINGLE_LT||val->type()==ARpcSensor::SINGLE_GT)
	{
		const ARpcSingleSensorValue *val2=(const ARpcSingleSensorValue*)val;
		qint64 t;
		if(val->type()==ARpcSensor::SINGLE_GT)t=val2->time();
		else t=QDateTime::currentMSecsSinceEpoch();
		//IMPL
	}
	//IMPL else
}

ARpcISensorValue *ARpcLastNValuesStorage::valFromData(const QByteArray &data)
{
	//IMPL
}

