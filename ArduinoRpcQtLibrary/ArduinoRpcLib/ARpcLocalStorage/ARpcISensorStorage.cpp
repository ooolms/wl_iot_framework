#include "ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcContinuousStorage.h"
#include <QDir>
#include <QSettings>

ARpcISensorStorage* ARpcISensorStorage::preCreate(const QString &path,ARpcISensorStorage::StoreMode mode)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	QSettings file(dir.absolutePath()+"/storage.ini",QSettings::IniFormat);
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
	file.sync();

	return makeStorage(mode);
}

ARpcISensorStorage* ARpcISensorStorage::open(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())return 0;
	QDir dir(path);
	if(!dir.exists())return 0;
	QSettings file(dir.absolutePath()+"/storage.ini",QSettings::IniFormat);
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
	ARpcISensorStorage *st=makeStorage(mode);
	if(!st)return 0;
	if(!st->openInternal(path))
	{
		delete st;
		return 0;
	}
	return st;
}

ARpcISensorStorage* ARpcISensorStorage::makeStorage(ARpcISensorStorage::StoreMode mode)
{
	//TODO add storages
	if(mode==CONTINUOUS)return new ARpcContinuousStorage;
	else return 0;
}
