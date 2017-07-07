#include "ARpcSessionStorage.h"
#include "ARpcDBDriverHelpers.h"

ARpcSessionStorage::ARpcSessionStorage(bool autoSess,ARpcSensor::Type valType,QObject *parent)
	:ARpcISensorStorage(valType,parent)
{
	autoSessions=autoSess;
	opened=false;
	hasIndex=false;
	dbType=CHAINED_BLOCKS;
	effectiveValType=valueType;
}

ARpcSessionStorage::~ARpcSessionStorage()
{
	close();
}

ARpcISensorStorage::StoreMode ARpcSessionStorage::getStoreMode()const
{
	if(autoSessions)return ARpcISensorStorage::AUTO_SESSIONS;
	else return ARpcISensorStorage::MANUAL_SESSIONS;
}

bool ARpcSessionStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened||mainWriteSessionId.isNull())return false;
	if(val->type()!=valueType)return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp.packSensorValue(val,hasTime,ts);
	if(data.isEmpty())return false;
	if(dbType==FIXED_BLOCKS)
	{
		if(mainWriteSession.fbDb->writeBlock(data))
		{
			if(hasIndex)
				mainWriteSession.indDb->append(ts,mainWriteSession.fbDb->blocksCount()-1);
			emit newValueWritten(val);
			return true;
		}
		return false;
	}
	else
	{
		if(mainWriteSession.cbDb->writeBlock(data))
		{
			if(hasIndex)
				mainWriteSession.indDb->append(ts,mainWriteSession.cbDb->blocksCount()-1);
			emit newValueWritten(val);
			return true;
		}
		return false;
	}
}

ARpcISensorStorage::TimestampRule ARpcSessionStorage::fixTimestampRule(ARpcISensorStorage::TimestampRule rule)
{
	return rule;
}

bool ARpcSessionStorage::createAsFixedBlocksDb(const ARpcISensorValue &templateValue,TimestampRule rule,bool gtIndex)
{
	if(opened)return false;
	blockNoteSizesForSessions=ARpcDBDriverHelpers(rule).sizesForFixedBlocksDb(templateValue);
	dbDir.mkdir("sessions");
	if(!dbDir.cd("sessions"))return false;
	if(!dbDir.cdUp())return false;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hasIndex=gtIndex&&(effectiveValType==ARpcSensor::TEXT||effectiveValType==ARpcSensor::SINGLE_GT||
		effectiveValType==ARpcSensor::PACKET_GT);
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","fixed_blocks");
	settings.setValue("blockNoteSizes",blockNoteSizesToString());
	settings.setValue("gt_index",hasIndex?"1":"0");
	settings.sync();
	dbType=FIXED_BLOCKS;
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

bool ARpcSessionStorage::createAsChainedBlocksDb(TimestampRule rule,bool gtIndex)
{
	if(opened)return false;
	dbDir.mkdir("sessions");
	if(!dbDir.cd("sessions"))return false;
	if(!dbDir.cdUp())return false;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hasIndex=(gtIndex&&(effectiveValType==ARpcSensor::TEXT||effectiveValType==ARpcSensor::SINGLE_GT||
		effectiveValType==ARpcSensor::PACKET_GT));
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","chained_blocks");
	settings.setValue("gt_index",hasIndex?"1":"0");
	settings.sync();
	dbType=CHAINED_BLOCKS;
	hlp=ARpcDBDriverHelpers(timestampRule);
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

bool ARpcSessionStorage::open()
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
	hasIndex=(settings.value("gt_index").toString()=="1");
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

void ARpcSessionStorage::closeInternal()
{
	if(!opened)return;
	for(Session &d:sessions)
		closeSessionAndDeleteDb(d);
	sessions.clear();
	if(!mainWriteSessionId.isNull())
	{
		closeSessionAndDeleteDb(mainWriteSession);
		mainWriteSessionId=QUuid();
	}
	opened=false;
}

ARpcSensor::Type ARpcSessionStorage::effectiveValuesType()const
{
	return effectiveValType;
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

void ARpcSessionStorage::closeSessionAndDeleteDb(ARpcSessionStorage::Session &d)
{
	if(dbType==FIXED_BLOCKS)
	{
		d.fbDb->close();
		d.indDb->close();
		delete d.fbDb;
		delete d.indDb;
	}
	else
	{
		d.cbDb->close();
		d.indDb->close();
		delete d.cbDb;
		delete d.indDb;
	}
	d.attributes.clear();
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
	id=QUuid::createUuid();
	while(sessionsDir.exists(id.toString()))
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
	if(hasIndex)
	{
		ARpcDBDriverGTimeIndex indDb;
		created=created&&indDb.create(sessionsDir.absolutePath()+"/"+idStr+"/index.db");
	}
	if(!created)
	{
		file.remove();
		sessionsDir.rmdir(idStr);
	}
	return created;
}

bool ARpcSessionStorage::openMainWriteSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())return false;
	if(!mainWriteSessionId.isNull())return false;
	QDir sessionDir=dbDir;
	if(!sessionDir.cd("sessions"))return false;
	if(!sessionDir.cd(sessionId.toString()))return false;
	mainWriteSession.indDb=new ARpcDBDriverGTimeIndex(this);
	if(hasIndex&&!mainWriteSession.indDb->open(sessionDir.absolutePath()+"/index.db"))
	{
		delete mainWriteSession.indDb;
		return false;
	}
	if(dbType==FIXED_BLOCKS)
	{
		mainWriteSession.fbDb=new ARpcDBDriverFixedBlocks(this);
		if(!mainWriteSession.fbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete mainWriteSession.fbDb;
			delete mainWriteSession.indDb;
			return false;
		}
	}
	else
	{
		mainWriteSession.cbDb=new ARpcDBDriverChainedBlocks(this);
		if(!mainWriteSession.cbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete mainWriteSession.cbDb;
			delete mainWriteSession.indDb;
			return false;
		}
	}
	QSettings attrs(sessionDir.absolutePath()+"/metadata.ini",QSettings::IniFormat);
	QStringList keys=attrs.allKeys();
	mainWriteSession.attributes.clear();
	for(QString &k:keys)
		mainWriteSession.attributes[k]=attrs.value(k);
	mainWriteSessionId=sessionId;
	return true;
}

bool ARpcSessionStorage::openSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())return false;
	if(mainWriteSessionId==sessionId||sessions.contains(sessionId))return true;
	QDir sessionDir=dbDir;
	if(!sessionDir.cd("sessions"))return false;
	if(!sessionDir.cd(sessionId.toString()))return false;
	Session s;
	s.indDb=new ARpcDBDriverGTimeIndex(this);
	if(hasIndex&&!s.indDb->open(sessionDir.absolutePath()+"/index.db"))
	{
		delete s.indDb;
		return false;
	}
	if(dbType==FIXED_BLOCKS)
	{
		s.fbDb=new ARpcDBDriverFixedBlocks(this);
		if(!s.fbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete s.fbDb;
			delete s.indDb;
			return false;
		}
	}
	else
	{
		s.cbDb=new ARpcDBDriverChainedBlocks(this);
		if(!s.cbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete s.cbDb;
			delete s.indDb;
			return false;
		}
	}
	QSettings attrs(sessionDir.absolutePath()+"/metadata.ini",QSettings::IniFormat);
	QStringList keys=attrs.allKeys();
	s.attributes.clear();
	for(QString &k:keys)
		s.attributes[k]=attrs.value(k);
	sessions[sessionId]=s;
	return true;
}

bool ARpcSessionStorage::closeMainWriteSession()
{
	if(mainWriteSessionId.isNull())return true;
	closeSessionAndDeleteDb(mainWriteSession);
	mainWriteSessionId=QUuid();
	return true;
}

bool ARpcSessionStorage::closeSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())return false;
	if(sessions.contains(sessionId))
	{
		closeSessionAndDeleteDb(sessions[sessionId]);
		sessions.remove(sessionId);
	}
	return true;
}

bool ARpcSessionStorage::removeSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())return false;
	if(sessions.contains(sessionId)||mainWriteSessionId==sessionId)return false;
	QDir sessionsDir=dbDir;
	if(!sessionsDir.cd("sessions"))return false;
	QString idStr=sessionId.toString();
	if(!sessionsDir.exists(idStr))return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/data.db").remove())return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/metadata.ini").remove())return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/title.txt").remove())return false;
	return sessionsDir.rmdir(idStr);
}

bool ARpcSessionStorage::setSessionAttribute(const QUuid &sessionId,const QString &key,const QVariant &val)
{
	if(!opened||sessionId.isNull())return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)return false;
	QSettings attrs(dbDir.absolutePath()+"/sessions/"+sessionId.toString()+"/metadata.ini",QSettings::IniFormat);
	attrs.setValue(key,val);
	attrs.sync();
	if(attrs.status()!=QSettings::NoError)return false;
	if(mainWriteSessionId==sessionId)
		mainWriteSession.attributes[key]=val;
	else sessions[sessionId].attributes[key]=val;
	return true;
}

bool ARpcSessionStorage::getSessionAttribute(const QUuid &sessionId,const QString &key,QVariant &val)
{
	if(!opened||sessionId.isNull())return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)return false;
	if(mainWriteSessionId==sessionId)
	{
		if(!mainWriteSession.attributes.contains(key))return false;
		val=mainWriteSession.attributes[key];
		return true;
	}
	else
	{
		Session &d=sessions[sessionId];
		if(!d.attributes.contains(key))return false;
		val=d.attributes[key];
		return true;
	}
	return false;
}

quint64 ARpcSessionStorage::valuesCount(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())return 0;
	if(!mainWriteSessionId.isNull()&&mainWriteSessionId==sessionId)
	{
		if(dbType==FIXED_BLOCKS)return mainWriteSession.fbDb->blocksCount();
		else return mainWriteSession.cbDb->blocksCount();
	}
	else
	{
		Session &d=sessions[sessionId];
		if(dbType==FIXED_BLOCKS)return d.fbDb->blocksCount();
		else return d.cbDb->blocksCount();
	}
	return 0;
}

quint64 ARpcSessionStorage::findInGTIndex(const QUuid &sessionId,qint64 ts)
{
	if(!opened||!hasIndex||sessionId.isNull())return 0;
	if(mainWriteSessionId==sessionId)
		return mainWriteSession.indDb->findIndex(ts);
	else if(sessions.contains(sessionId))
		return sessions[sessionId].indDb->findIndex(ts);
	else return 0;
}

ARpcISensorValue* ARpcSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!opened||sessionId.isNull())return 0;
	if(mainWriteSessionId==sessionId)
	{
		QByteArray data;
		if(dbType==FIXED_BLOCKS&&!mainWriteSession.fbDb->readBlock(index,data))return 0;
		else if(dbType==CHAINED_BLOCKS&&!mainWriteSession.cbDb->readBlock((quint32)index,data))return 0;
		return hlp.unpackSensorValue(effectiveValType,data);
	}
	else if(sessions.contains(sessionId))
	{
		Session &d=sessions[sessionId];
		QByteArray data;
		if(dbType==FIXED_BLOCKS&&!d.fbDb->readBlock(index,data))return 0;
		else if(dbType==CHAINED_BLOCKS&&!d.cbDb->readBlock((quint32)index,data))return 0;
		return hlp.unpackSensorValue(effectiveValType,data);
	}
	return 0;
}

bool ARpcSessionStorage::isSessionOpened(const QUuid &sessionId)const
{
	return opened&&(mainWriteSessionId==sessionId||sessions.contains(sessionId));
}

bool ARpcSessionStorage::isMainWriteSessionOpened()const
{
	return !mainWriteSessionId.isNull();
}

bool ARpcSessionStorage::isOpened()const
{
	return opened;
}

QUuid ARpcSessionStorage::getMainWriteSessionId()const
{
	return mainWriteSessionId;
}
