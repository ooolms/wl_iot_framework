/*******************************************
   Copyright 2017 OOO "LMS"

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.*/

#include "ARpcSessionStorage.h"
#include "ARpcDBDriverHelpers.h"

ARpcSessionStorage::ARpcSessionStorage(const QString &path,bool autoSess,const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:ARpcISessionSensorStorage(devId,devName,sensor,autoSess?AUTO_SESSIONS:MANUAL_SESSIONS,tsRule,
		defaultStoredValuesType(sensor.type,tsRule),parent)
	,fsStorageHelper(path)
	,hlp(tsRule)
{
	autoSessions=autoSess;
	opened=false;
	hasIndex=false;
	dbType=CHAINED_BLOCKS;
}

ARpcSessionStorage::~ARpcSessionStorage()
{
	close();
}

bool ARpcSessionStorage::create(bool gtIndex)
{
	if(mSensor.type.hasFixedSize())
		return createAsFixedBlocksDb(gtIndex);
	else return createAsChainedBlocksDb(gtIndex);
}

bool ARpcSessionStorage::writeSensorValue(const ARpcSensorValue *val)
{
	if(!opened||mainWriteSessionId.isNull())
		return false;
	if(val->type()!=mSensor.type)
		return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp.packSensorValue(val,hasTime,ts);
	if(data.isEmpty())
		return false;
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

bool ARpcSessionStorage::createAsFixedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	QDir dir(fsStorageHelper.dbPath());
	dir.mkdir("sessions");
	if(!dir.cd("sessions"))
		return false;
	if(!dir.cdUp())
		return false;
	hasIndex=gtIndex&&(mStoredValuesType.tsType==ARpcSensorDef::GLOBAL_TIME);
	fsStorageHelper.settings()->setValue("db_type","fixed_blocks");
	fsStorageHelper.settings()->setValue("gt_index",hasIndex?"1":"0");
	fsStorageHelper.settings()->sync();
	dbType=FIXED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcSessionStorage::createAsChainedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	QDir dir(fsStorageHelper.dbPath());
	dir.mkdir("sessions");
	if(!dir.cd("sessions"))
		return false;
	if(!dir.cdUp())
		return false;
	hasIndex=gtIndex&&(mStoredValuesType.tsType==ARpcSensorDef::GLOBAL_TIME);
	fsStorageHelper.settings()->setValue("db_type","chained_blocks");
	fsStorageHelper.settings()->setValue("gt_index",hasIndex?"1":"0");
	fsStorageHelper.settings()->sync();
	dbType=CHAINED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcSessionStorage::open()
{
	if(opened)
		return false;
	QString dbTypeStr=fsStorageHelper.settings()->value("db_type").toString();
	if(dbTypeStr=="fixed_blocks")
		dbType=FIXED_BLOCKS;
	else if(dbTypeStr=="chained_blocks")
		dbType=CHAINED_BLOCKS;
	else
		return false;
	hasIndex=(fsStorageHelper.settings()->value("gt_index").toString()=="1");
	opened=true;
	return true;
}

void ARpcSessionStorage::close()
{
	if(!opened)
		return;
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

bool ARpcSessionStorage::listSessions(QList<QUuid> &ids,QByteArrayList &titles)
{
	if(!opened)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper.dbPath());
	if(!sessionsDir.cd("sessions"))
		return false;
	QStringList entries=sessionsDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	ids.clear();
	titles.clear();
	for(QString &s:entries)
	{
		QUuid id(s);
		if(id.isNull())
			continue;
		QFile file(sessionsDir.absolutePath()+"/"+s+"/title.txt");
		if(!file.open(QIODevice::ReadOnly))
			continue;
		QByteArray title=file.readAll();
		file.close();
		ids.append(id);
		titles.append(title);
	}
	return true;
}

bool ARpcSessionStorage::createSession(const QByteArray &title,QUuid &id)
{
	if(!opened)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper.dbPath());
	if(!sessionsDir.cd("sessions"))
		return false;
	id=QUuid::createUuid();
	while(sessionsDir.exists(id.toString()))
		id=QUuid::createUuid();
	QString idStr=id.toString();
	if(!sessionsDir.mkdir(idStr))
		return false;
	QFile file(sessionsDir.absolutePath()+"/"+idStr+"/title.txt");
	if(!file.open(QIODevice::WriteOnly))
	{
		sessionsDir.rmdir(idStr);
		return false;
	}
	file.write(title);
	file.close();
	bool created=false;
	if(dbType==FIXED_BLOCKS)
	{
		ARpcDBDriverFixedBlocks tmpDrv;
		created=tmpDrv.create(sessionsDir.absolutePath()+"/"+idStr+"/data.db",
			ARpcDBDriverHelpers::sizesForFixedBlocksDb(mStoredValuesType));
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
	if(!opened||sessionId.isNull())
		return false;
	if(!mainWriteSessionId.isNull())
		return false;
	QDir sessionDir=QDir(fsStorageHelper.dbPath());
	if(!sessionDir.cd("sessions"))
		return false;
	if(!sessionDir.cd(sessionId.toString()))
		return false;
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
		mainWriteSession.attributes[k.toUtf8()]=attrs.value(k).toByteArray();
	mainWriteSessionId=sessionId;
	return true;
}

bool ARpcSessionStorage::openSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(mainWriteSessionId==sessionId||sessions.contains(sessionId))
		return true;
	QDir sessionDir=QDir(fsStorageHelper.dbPath());
	if(!sessionDir.cd("sessions"))
		return false;
	if(!sessionDir.cd(sessionId.toString()))
		return false;
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
		s.attributes[k.toUtf8()]=attrs.value(k).toByteArray();
	sessions[sessionId]=s;
	return true;
}

bool ARpcSessionStorage::closeMainWriteSession()
{
	if(mainWriteSessionId.isNull())
		return true;
	closeSessionAndDeleteDb(mainWriteSession);
	mainWriteSessionId=QUuid();
	return true;
}

bool ARpcSessionStorage::closeSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(sessions.contains(sessionId))
	{
		closeSessionAndDeleteDb(sessions[sessionId]);
		sessions.remove(sessionId);
	}
	if(mainReadSessionId==sessionId)
		mainReadSessionId=QUuid();
	return true;
}

bool ARpcSessionStorage::removeSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(sessions.contains(sessionId)||mainWriteSessionId==sessionId)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper.dbPath());
	if(!sessionsDir.cd("sessions"))
		return false;
	QString idStr=sessionId.toString();
	if(!sessionsDir.exists(idStr))
		return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/data.db").remove())
		return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/metadata.ini").remove())
		return false;
	if(!QFile(sessionsDir.absolutePath()+"/"+idStr+"/title.txt").remove())
		return false;
	return sessionsDir.rmdir(idStr);
}

bool ARpcSessionStorage::setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	QSettings attrs(fsStorageHelper.dbPath()+"/sessions/"+sessionId.toString()+"/metadata.ini",QSettings::IniFormat);
	attrs.setValue(QString::fromUtf8(key),val);
	attrs.sync();
	if(attrs.status()!=QSettings::NoError)
		return false;
	if(mainWriteSessionId==sessionId)
		mainWriteSession.attributes[key]=val;
	else
		sessions[sessionId].attributes[key]=val;
	return true;
}

bool ARpcSessionStorage::removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	QSettings attrs(fsStorageHelper.dbPath()+"/sessions/"+sessionId.toString()+"/metadata.ini",QSettings::IniFormat);
	attrs.remove(QString::fromUtf8(key));
	attrs.sync();
	if(attrs.status()!=QSettings::NoError)
		return false;
	if(mainWriteSessionId==sessionId)
		mainWriteSession.attributes.remove(key);
	else
		sessions[sessionId].attributes.remove(key);
	return true;
}

bool ARpcSessionStorage::getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	if(mainWriteSessionId==sessionId)
	{
		if(!mainWriteSession.attributes.contains(key))
			return false;
		val=mainWriteSession.attributes[key];
		return true;
	}
	else
	{
		Session &d=sessions[sessionId];
		if(!d.attributes.contains(key))
			return false;
		val=d.attributes[key];
		return true;
	}
	return false;
}

bool ARpcSessionStorage::listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	if(mainWriteSessionId==sessionId)
	{
		map=mainWriteSession.attributes;
		return true;
	}
	else
	{
		Session &d=sessions[sessionId];
		map=d.attributes;
		return true;
	}
	return false;
}

quint64 ARpcSessionStorage::valuesCount(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return 0;
	if(!mainWriteSessionId.isNull()&&mainWriteSessionId==sessionId)
	{
		if(dbType==FIXED_BLOCKS)
			return mainWriteSession.fbDb->blocksCount();
		else
			return mainWriteSession.cbDb->blocksCount();
	}
	else
	{
		Session &d=sessions[sessionId];
		if(dbType==FIXED_BLOCKS)
			return d.fbDb->blocksCount();
		else
			return d.cbDb->blocksCount();
	}
	return 0;
}

quint64 ARpcSessionStorage::findInGTIndex(const QUuid &sessionId,qint64 ts)
{
	if(!opened||!hasIndex||sessionId.isNull())
		return 0;
	if(mainWriteSessionId==sessionId)
		return mainWriteSession.indDb->findIndex(ts);
	else if(sessions.contains(sessionId))
		return sessions[sessionId].indDb->findIndex(ts);
	else
		return 0;
}

ARpcSensorValue *ARpcSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!opened||sessionId.isNull())
		return 0;
	if(mainWriteSessionId==sessionId)
	{
		QByteArray data;
		if(dbType==FIXED_BLOCKS&&!mainWriteSession.fbDb->readBlock(index,data))
			return 0;
		else if(dbType==CHAINED_BLOCKS&&!mainWriteSession.cbDb->readBlock((quint32)index,data))
			return 0;
		return hlp.unpackSensorValue(mStoredValuesType,data);
	}
	else if(sessions.contains(sessionId))
	{
		Session &d=sessions[sessionId];
		QByteArray data;
		if(dbType==FIXED_BLOCKS&&!d.fbDb->readBlock(index,data))
			return 0;
		else if(dbType==CHAINED_BLOCKS&&!d.cbDb->readBlock((quint32)index,data))
			return 0;
		return hlp.unpackSensorValue(mStoredValuesType,data);
	}
	return 0;
}

bool ARpcSessionStorage::isSessionOpened(const QUuid &sessionId) const
{
	return opened&&(mainWriteSessionId==sessionId||sessions.contains(sessionId));
}

bool ARpcSessionStorage::isMainWriteSessionOpened() const
{
	return !mainWriteSessionId.isNull();
}

bool ARpcSessionStorage::isOpened() const
{
	return opened;
}

quint64 ARpcSessionStorage::valuesCount()
{
	return valuesCount(mainReadSessionId);
}

ARpcSensorValue* ARpcSessionStorage::valueAt(quint64 index)
{
	return valueAt(mainReadSessionId,index);
}

QUuid ARpcSessionStorage::getMainWriteSessionId() const
{
	return mainWriteSessionId;
}

bool ARpcSessionStorage::setMainReadSessionId(const QUuid &id)
{
	if(!opened)
		return false;
	if(id.isNull())
	{
		mainReadSessionId=id;
		return true;
	}
	if(!sessions.contains(id)&&!openSession(id))
		return false;
	mainReadSessionId=id;
	return true;
}


void ARpcSessionStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	fsStorageHelper.writeAttribute(str,var);
}

QVariant ARpcSessionStorage::readAttribute(const QByteArray &str)
{
	return fsStorageHelper.readAttribute(str);
}

void ARpcSessionStorage::addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)
{
	fsStorageHelper.addDataExportConfig(serviceType,cfg);
}

bool ARpcSessionStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper.hasDataExportConfig(serviceType);
}

ARpcISensorStorage::DataExportConfig ARpcSessionStorage::getDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper.getDataExportConfig(serviceType);
}

void ARpcSessionStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	fsStorageHelper.removeDataExportConfig(serviceType);
}

QByteArrayList ARpcSessionStorage::allDataExportServices()
{
	return fsStorageHelper.allDataExportServices();
}

bool ARpcSessionStorage::values(quint64 index,quint64 count,quint64 step,
	VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!opened||mainReadSessionId.isNull())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *v=valueAt(mainReadSessionId,index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}
