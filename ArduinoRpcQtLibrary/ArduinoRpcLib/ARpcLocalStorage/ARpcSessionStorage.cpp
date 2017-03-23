#include "ARpcSessionStorage.h"
#include "ARpcDBDriverHelpers.h"

ARpcSessionStorage::ARpcSessionStorage(bool autoSess,QObject *parent)
	:ARpcISensorStorage(parent)
{
	autoSessions=autoSess;
	opened=false;
	sessionOpened=false;
	dbType=CHAINED_BLOCKS;
}

ARpcISensorStorage::StoreMode ARpcSessionStorage::getStoreMode()const
{
	if(autoSessions)return ARpcISensorStorage::AUTO_SESSIONS;
	else return ARpcISensorStorage::MANUAL_SESSIONS;
}

bool ARpcSessionStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!sessionOpened)return false;
	if(val->type()!=valueType)return false;
	if(dbType==FIXED_BLOCKS)return writeSensorValueFB(val);
	else return writeSensorValueCB(val);
}

bool ARpcSessionStorage::createAsFixedBlocksDb(const QVector<quint32> &blockNotesSizes)
{
	if(opened)return false;
	blockNoteSizesForSessions=blockNotesSizes;
	if(dbDir.exists("sessions")&&!dbDir.cd("sessions"))return false;
	else if(!dbDir.exists("sessions")&&!dbDir.mkdir("sessions")&&!dbDir.cd("sessions"))return false;
	if(!dbDir.cdUp())return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","fixed_blocks");
	settings.setValue("blockNoteSizes",blockNoteSizesToString());
	settings.sync();
	dbType=FIXED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcSessionStorage::createAsChainedBlocksDb()
{
	if(opened)return false;
	if(dbDir.exists("sessions")&&!dbDir.cd("sessions"))return false;
	else if(!dbDir.exists("sessions")&&!dbDir.mkdir("sessions")&&!dbDir.cd("sessions"))return false;
	if(!dbDir.cdUp())return false;
	if(!cbDb->create(dbDir.absolutePath()+"/data.db"))return true;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","chained_blocks");
	settings.sync();
	dbType=CHAINED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcSessionStorage::isFixesBlocksDb()const
{
	return dbType==FIXED_BLOCKS;
}

bool ARpcSessionStorage::isChainedBlocksDb()const
{
	return dbType==CHAINED_BLOCKS;
}

bool ARpcSessionStorage::openInternal()
{
	if(opened)return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	QString dbTypeStr=settings.value("db_type").toString();
	if(dbTypeStr=="fixed_blocks")
	{
		dbType=FIXED_BLOCKS;
		QString blockNoteSizesStr=settings.value("blockNoteSizes").toString();
		if(!parseBlockNoteSizes(blockNoteSizesStr))return false;
	}
	else if(dbTypeStr=="chained_blocks")
		dbType=CHAINED_BLOCKS;
	else return false;
	opened=true;
	//TODO list sessions?
	return true;
}

void ARpcSessionStorage::closeInternal()
{
	if(!opened)return;
	if(sessionOpened)
	{
		if(dbType==FIXED_BLOCKS)fbDb->close();
		else cbDb->close();
	}
	opened=false;
	sessionOpened=false;
}

bool ARpcSessionStorage::writeSensorValueFB(const ARpcISensorValue *val)
{
	return ARpcDBDriverHelpers::writeSensorValueToFixedBlocksDB(fbDb,val,false);
}

bool ARpcSessionStorage::writeSensorValueCB(const ARpcISensorValue *val)
{
	return ARpcDBDriverHelpers::writeSensorValueToChainedBlocksDB(cbDb,val,false);
}

QString ARpcSessionStorage::blockNoteSizesToString()
{
	QStringList retVal;
	for(quint32 sz:blockNoteSizesForSessions)
		retVal.append(QString::number(sz));
	return retVal.join(";");
}

bool ARpcSessionStorage::parseBlockNoteSizes(const QString &str)
{
	QStringList strVals=str.split(";",QString::SkipEmptyParts);
	bool ok=false;
	QVector<quint32> newSizes;
	if(strVals.isEmpty())return false;
	for(QString &s:strVals)
	{
		newSizes.append(s.toULong(&ok));
		if(!ok)return false;
	}
	blockNoteSizesForSessions=newSizes;
	return true;
}

bool ARpcSessionStorage::listSessions(QList<QUuid> &ids,QStringList &titles)
{
	if(!opened)return false;
	QDir sessionsDir=dbDir;
	if(!sessionsDir.cd("sessions"))return false;
	QStringList entries=sessionsDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	ids.clear();
	titles.clear();
	for(QString &s:entries)
	{
		QUuid id(s);
		if(id.isNull())continue;
		QFile file(sessionsDir.absolutePath()+"/"+s+"/title.txt");
		if(!file.open(QIODevice::ReadOnly))continue;
		QString title=QString::fromUtf8(file.readAll());
		file.close();
		ids.append(id);
		titles.append(title);
	}
	return true;
}

bool ARpcSessionStorage::createSession(const QString &title,QUuid &id)
{
	if(!opened)return false;
	QDir sessionsDir=dbDir;
	if(!sessionsDir.cd("sessions"))return false;
	while(!sessionsDir.exists(id.toString()))
		id=QUuid::createUuid();
	QString idStr=id.toString();
	if(!sessionsDir.mkdir(idStr))return false;
	QFile file(sessionsDir.absolutePath()+"/"+idStr+"/title.txt");
	if(!file.open(QIODevice::WriteOnly))
	{
		sessionsDir.rmdir(idStr);
		return false;
	}
	file.write(title.toUtf8());
	file.close();
	bool created=false;
	if(dbType==FIXED_BLOCKS)
	{
		ARpcDBDriverFixedBlocks tmpDrv;
		created=tmpDrv.create(sessionsDir.absolutePath()+"/"+idStr+"/data.db",blockNoteSizesForSessions);
	}
	else
	{
		ARpcDBDriverChainedBlocks tmpDrv;
		created=tmpDrv.create(sessionsDir.absolutePath()+"/"+idStr+"/data.db");
	}
	if(!created)
	{
		file.remove();
		sessionsDir.rmdir(idStr);
	}
	return created;
}

bool ARpcSessionStorage::openSession(const QUuid &id)
{
	if(!opened)return false;
	QDir sessionsDir=dbDir;
	if(!sessionsDir.cd("sessions"))return false;
	if(!sessionsDir.cd(id.toString()))return false;
	if(sessionOpened)
	{
		if(dbType==FIXED_BLOCKS)fbDb->close();
		else cbDb->close();
		sessionOpened=false;
		currentSessionId=QUuid();
		sessionAttrs.clear();
	}
	if(dbType==FIXED_BLOCKS&&!fbDb->open(sessionsDir.absolutePath()+"/data.db"))return false;
	else if(!cbDb->open(sessionsDir.absolutePath()+"/data.db"))return false;
	sessionOpened=true;
	currentSessionId=id;
	QSettings attrs(sessionsDir.absolutePath()+"/metadata.ini",QSettings::IniFormat);
	QStringList keys=attrs.allKeys();
	sessionAttrs.clear();
	for(QString &k:keys)
		sessionAttrs[k]=attrs.value(k);
	return true;
}

bool ARpcSessionStorage::closeSession()
{
	if(!opened)return false;
	if(sessionOpened)
	{
		if(dbType==FIXED_BLOCKS)fbDb->close();
		else cbDb->close();
		sessionOpened=false;
		currentSessionId=QUuid();
		sessionAttrs.clear();
	}
	return true;
}

bool ARpcSessionStorage::removeSession(const QUuid &id)
{
	if(!opened)return false;
	if(sessionOpened&&currentSessionId==id)closeSession();
	QDir sessionsDir=dbDir;
	if(!sessionsDir.cd("sessions"))return false;
	QString idStr=id.toString();
	if(!sessionsDir.exists(idStr))return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/data.db").remove())return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/metadata.ini").remove())return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/title.txt").remove())return false;
	return true;
}

bool ARpcSessionStorage::setSessionAttribute(const QString &key,const QVariant &val)
{
	if(!sessionOpened)return false;
	QSettings attrs(dbDir.absolutePath()+"/sessions/"+currentSessionId.toString()+"/metadata.ini",QSettings::IniFormat);
	attrs.setValue(key,val);
	attrs.sync();
	if(attrs.status()!=QSettings::NoError)return false;
	sessionAttrs[key]=val;
	return true;
}

bool ARpcSessionStorage::getSessionAttribute(const QString &key,QVariant &val)
{
	if(!sessionOpened)return false;
	if(!sessionAttrs.contains(key))return false;
	val=sessionAttrs[key];
	return true;
}

qint64 ARpcSessionStorage::valuesCount()
{
	if(!opened||!sessionOpened)return 0;
	if(dbType==FIXED_BLOCKS)return fbDb->blocksCount();
	else return cbDb->blocksCount();
}

ARpcISensorValue *ARpcSessionStorage::valueAt(quint64 index)
{
	if(!opened||!sessionOpened)return 0;
	if(dbType==FIXED_BLOCKS)return ARpcDBDriverHelpers::readSensorValueFromFixedBlocksDB(fbDb,valueType,index);
	else return ARpcDBDriverHelpers::readSensorValueFromChainedBlocksDB(cbDb,valueType,index);
}
