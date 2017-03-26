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
	QSettings file(dir.absolutePath()+"/storage.ini",QSettings::IniFormat);

	//mode
	if(mode==CONTINUOUS)
		file.setValue("mode","continuous");
	else if(mode==MANUAL_SESSIONS)
		file.setValue("mode","manual_sessions");
	else if(mode==AUTO_SESSIONS)
		file.setValue("mode","auto_sessions");
	else if(mode==LAST_N_VALUES)
		file.setValue("mode","last_n_values");

	//valType
	if(valType==ARpcSensor::SINGLE)
		file.setValue("value_type","single");
	else if(valType==ARpcSensor::SINGLE_LT)
		file.setValue("value_type","single_lt");
	else if(valType==ARpcSensor::SINGLE_GT)
		file.setValue("value_type","single_gt");
	else if(valType==ARpcSensor::TEXT)
		file.setValue("value_type","text");
	else if(valType==ARpcSensor::PACKET)
		file.setValue("value_type","packet");
	else if(valType==ARpcSensor::PACKET_LT)
		file.setValue("value_type","packet_lt");
	else if(valType==ARpcSensor::PACKET_GT)
		file.setValue("value_type","packet_lt");
	file.sync();

	ARpcISensorStorage *st=makeStorage(valType,mode);
	st->dbDir=dir;
	return st;
}

ARpcISensorStorage* ARpcISensorStorage::open(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir=QDir(path);
	if(!dir.exists())return 0;
	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);

	//mode
	StoreMode mode=LAST_N_VALUES;
	QString strValue=file.value("mode").toString();
	if(strValue=="continuous")
		mode=CONTINUOUS;
	else if(strValue=="manual_sessions")
		mode=MANUAL_SESSIONS;
	else if(strValue=="auto_sessions")
		mode=AUTO_SESSIONS;
	else if(strValue=="last_n_values")
		mode=LAST_N_VALUES;
	else return 0;

	//value_type
	strValue=file.value("value_type").toString();
	ARpcSensor::Type valType;
	if(strValue=="single")
		valType=ARpcSensor::SINGLE;
	else if(strValue=="single_lt")
		valType=ARpcSensor::SINGLE_LT;
	else if(strValue=="single_gt")
		valType=ARpcSensor::SINGLE_GT;
	else if(strValue=="text")
		valType=ARpcSensor::TEXT;
	else if(strValue=="packet")
		valType=ARpcSensor::PACKET;
	else if(strValue=="packet_lt")
		valType=ARpcSensor::PACKET_LT;
	else if(strValue=="packet_gt")
		valType=ARpcSensor::PACKET_GT;
	else return 0;

	ARpcISensorStorage *st=makeStorage(valType,mode);
	if(!st)return 0;
	st->dbDir=dir;
	if(!st->openInternal())
	{
		delete st;
		return 0;
	}
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
