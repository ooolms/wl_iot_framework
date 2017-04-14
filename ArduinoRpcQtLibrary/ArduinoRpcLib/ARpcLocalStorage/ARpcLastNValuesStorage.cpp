#include "ARpcLastNValuesStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include <QDateTime>

ARpcLastNValuesStorage::ARpcLastNValuesStorage(ARpcSensor::Type valType,QObject *parent)
	:ARpcISensorStorage(valType,parent)
{
	storedCount=0;
	opened=false;
	timestampRule=ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
}

ARpcLastNValuesStorage::~ARpcLastNValuesStorage()
{
	close();
}

bool ARpcLastNValuesStorage::create(quint32 storedValuesCount,const ARpcISensorValue &fillerValue,TimestampRule rule)
{
	if(opened)return false;
	if(storedValuesCount==0)storedValuesCount=1;
	timestampRule=rule;
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		timestampRule==ARpcISensorStorage::DONT_TOUCH)timestampRule=ARpcISensorStorage::ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	QFile file(dbDir.absolutePath()+"/db.index");
	if(file.exists())return false;
	storedCount=storedValuesCount;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("time_rule",timestampRuleToString(timestampRule));
	settings.setValue("stored_count",QString::number(storedCount));
	settings.sync();
	if(settings.status()!=QSettings::NoError)return false;
	if(!file.open(QIODevice::WriteOnly))return false;
	file.write("0");
	file.close();
	opened=true;
	hlp=ARpcDBDriverHelpers(timestampRule);
	startIndex=0;
	for(quint32 i=0;i<storedCount;++i)
		if(!writeSensorValue(&fillerValue))return false;
	return true;
}

ARpcISensorStorage::StoreMode ARpcLastNValuesStorage::getStoreMode()const
{
	return ARpcISensorStorage::LAST_N_VALUES;
}

bool ARpcLastNValuesStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)return false;
	QByteArray data=hlp.packSensorValue(val);
	if(data.isEmpty())return false;
	QFile dataFile(dbDir.absolutePath()+"/"+QString::number((startIndex+storedCount-1)%storedCount)+".data");
	QFile indexFile(dbDir.absolutePath()+"/db.index");
	if(!dataFile.open(QIODevice::WriteOnly)||!indexFile.open(QIODevice::WriteOnly))return false;
	indexFile.write(QByteArray::number((startIndex+storedCount-1)%storedCount));
	indexFile.close();
	startIndex=(startIndex+storedCount-1)%storedCount;
	if(dataFile.write(data)!=data.size())
	{
		dataFile.close();
		dataFile.remove();
	}
	else dataFile.close();
	return true;
}

ARpcISensorValue *ARpcLastNValuesStorage::valueAt(quint32 index)
{
	if(index>=storedCount)return 0;
	QFile file(dbDir.absolutePath()+"/"+QString::number((startIndex+index)%storedCount)+".data");
	if(!file.open(QIODevice::ReadOnly))return 0;
	QByteArray data=file.readAll();
	file.close();
	if(data.isEmpty())return 0;
	return hlp.unpackSensorValue(effectiveValType,data);
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
	storedCount=settings.value("stored_count").toUInt(&ok);
	if(!ok)return false;
	if(storedCount==0)storedCount=1;
	if(!file.open(QIODevice::ReadOnly))return false;
	startIndex=file.readAll().toULong(&ok);
	file.close();
	if(!ok)return false;
	if(!timestampRuleFromString(settings.value("time_rule").toString(),timestampRule))return false;
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		timestampRule==ARpcISensorStorage::DONT_TOUCH)timestampRule=ARpcISensorStorage::ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

void ARpcLastNValuesStorage::closeInternal()
{
	opened=false;
	storedCount=0;
	startIndex=0;
}

ARpcSensor::Type ARpcLastNValuesStorage::effectiveValuesType()const
{
	return effectiveValType;
}
