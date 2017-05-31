#include "ARpcISensorStorage.h"
#include "ARpcContinuousStorage.h"
#include "ARpcSessionStorage.h"
#include "ARpcLastNValuesStorage.h"
#include <QDir>
#include <QSettings>

ARpcISensorStorage::ARpcISensorStorage(ARpcSensor::Type valType,QObject *parent)
	:QObject(parent)
{
	valueType=valType;
}

ARpcISensorStorage* ARpcISensorStorage::preCreate(
	const QString &path,ARpcISensorStorage::StoreMode mode,ARpcSensor::Type valType)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.setValue("mode",storeModeToString(mode));
	file.setValue("value_type",ARpcSensor::typeToString(valType));
	file.sync();

	ARpcISensorStorage *st=makeStorage(valType,mode);
	st->dbDir=dir;
	return st;
}

ARpcISensorStorage* ARpcISensorStorage::preOpen(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir=QDir(path);
	if(!dir.exists())return 0;
	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	StoreMode mode=storeModeFromString(file.value("mode").toString());
	if(mode==BAD_MODE)return 0;
	ARpcSensor::Type valType=ARpcSensor::typeFromString(file.value("value_type").toString());
	if(valType==ARpcSensor::BAD_TYPE)return 0;

	ARpcISensorStorage *st=makeStorage(valType,mode);
	if(!st)return 0;
	st->dbDir=dir;
	return st;
}

ARpcSensor::Type ARpcISensorStorage::sensorValuesType()const
{
	return valueType;
}

QDir ARpcISensorStorage::getDbDir()const
{
	return dbDir;
}

QString ARpcISensorStorage::storeModeToString(ARpcISensorStorage::StoreMode mode)
{
	if(mode==CONTINUOUS)return "continuous";
	else if(mode==MANUAL_SESSIONS)return "manual_sessions";
	else if(mode==AUTO_SESSIONS)return "auto_sessions";
	else if(mode==LAST_N_VALUES)return "last_n_values";
	else return QString();
}

ARpcISensorStorage::StoreMode ARpcISensorStorage::storeModeFromString(const QString &str)
{
	if(str=="continuous")return CONTINUOUS;
	else if(str=="manual_sessions")return MANUAL_SESSIONS;
	else if(str=="auto_sessions")return AUTO_SESSIONS;
	else if(str=="last_n_values")return LAST_N_VALUES;
	else return BAD_MODE;
}

void ARpcISensorStorage::setDeviceName(const QString &name)
{
	if(!isOpened())return;
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.setValue("deviceName",name);
	file.sync();
}

QString ARpcISensorStorage::getDeviceName()
{
	if(!isOpened())return QString();
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	return file.value("deviceName").toString();
}

void ARpcISensorStorage::close()
{
	closeInternal();
	dbDir=QDir();
	valueType=ARpcSensor::BAD_TYPE;
}

QString ARpcISensorStorage::settingsFileRelPath()
{
	return "database.ini";
}

QString ARpcISensorStorage::timestampRuleToString(ARpcISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)return "add_global_time";
	else if(rule==DROP_TIME)return "drop_time";
	else /*DONT_TOUCH*/ return "dont_touch";
}

bool ARpcISensorStorage::timestampRuleFromString(const QString &str,ARpcISensorStorage::TimestampRule &rule)
{
	if(str=="add_global_time")rule=ADD_GT;
	else if(str=="drop_time")rule=DROP_TIME;
	else if(str=="dont_touch")rule=DONT_TOUCH;
	else return false;
	return true;
}

ARpcSensor::Type ARpcISensorStorage::defaultEffectiveValuesType(ARpcISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)
	{
		if(valueType==ARpcSensor::SINGLE||valueType==ARpcSensor::SINGLE_LT)return ARpcSensor::SINGLE_GT;
		else if(valueType==ARpcSensor::PACKET||valueType==ARpcSensor::PACKET_LT)return ARpcSensor::PACKET_GT;
		else return valueType;
	}
	else if(rule==DROP_TIME)
	{
		if(valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::SINGLE_GT)return ARpcSensor::SINGLE;
		else if(valueType==ARpcSensor::PACKET_LT||valueType==ARpcSensor::PACKET_GT)return ARpcSensor::PACKET;
		else return valueType;
	}
	else /*rule==DONT_TOUCH*/ return valueType;
}

ARpcISensorStorage* ARpcISensorStorage::makeStorage(ARpcSensor::Type valType,ARpcISensorStorage::StoreMode mode)
{
	if(mode==CONTINUOUS)return new ARpcContinuousStorage(valType);
	else if(mode==MANUAL_SESSIONS)return new ARpcSessionStorage(false,valType);
	else if(mode==AUTO_SESSIONS)return new ARpcSessionStorage(true,valType);
	else if(mode==LAST_N_VALUES)return new ARpcLastNValuesStorage(valType);
	else return 0;
}
