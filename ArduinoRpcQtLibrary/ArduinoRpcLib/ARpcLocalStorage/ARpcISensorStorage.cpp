#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcContinuousStorage.h"
#include <QDir>
#include <QSettings>

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
	else if(mode==LAST_VALUE)
		file.setValue("mode","last_value");
	else if(mode==LAST_PACKET)
		file.setValue("mode","last_packet");

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

	ARpcISensorStorage *st=makeStorage(mode);
	st->valueType=valType;
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
	StoreMode mode=LAST_VALUE;
	QString strValue=file.value("mode").toString();
	if(strValue=="continuous")
		mode=CONTINUOUS;
	else if(strValue=="manual_sessions")
		mode=MANUAL_SESSIONS;
	else if(strValue=="auto_sessions")
		mode=AUTO_SESSIONS;
	else if(strValue=="last_value")
		mode=LAST_VALUE;
	else if(strValue=="last_packet")
		mode=LAST_PACKET;
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

	ARpcISensorStorage *st=makeStorage(mode);
	if(!st)return 0;
	st->dbDir=dir;
	st->valueType=valType;
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

ARpcISensorStorage* ARpcISensorStorage::makeStorage(ARpcISensorStorage::StoreMode mode)
{
	//TODO add storages
	if(mode==CONTINUOUS)return new ARpcContinuousStorage;
	else return 0;
}
