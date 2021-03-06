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

#include "wliot/storages/SessionStorage.h"
#include "DBDriverHelpers.h"
#include "FSSensorStorageHelper.h"
#include "DBDriverFixedBlocks.h"
#include "DBDriverChainedBlocks.h"
#include "DBDriverGTimeIndex.h"

using namespace WLIOT;

SessionStorage::SessionStorage(const QString &path,bool autoSess,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sensor,TimestampRule tsRule,QObject *parent)
	:ISessionSensorStorage(devId,devName,sensor,autoSess?AUTO_SESSIONS:MANUAL_SESSIONS,tsRule,
		defaultStoredValuesType(sensor.type,tsRule),parent)
{
	autoSessions=autoSess;
	opened=false;
	mHasGTIndex=false;
	dbType=CHAINED_BLOCKS;
	hlp=new DBDriverHelpers(tsRule);
	fsStorageHelper=new FSSensorStorageHelper(path);
}

SessionStorage::~SessionStorage()
{
	close();
	delete fsStorageHelper;
	delete hlp;
}

bool SessionStorage::create(bool gtIndex)
{
	if(mSensor.type.hasFixedSize())
		return createAsFixedBlocksDb(gtIndex);
	else return createAsChainedBlocksDb(gtIndex);
}

bool SessionStorage::writeSensorValue(const SensorValue *val)
{
	if(!opened||mainWriteSessionId.isNull())
		return false;
	if(val->type()!=mSensor.type)
		return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp->packSensorValue(val,hasTime,ts);
	if(data.isEmpty())
		return false;
	if(dbType==FIXED_BLOCKS)
	{
		if(mainWriteSession.fbDb->writeBlock(data))
		{
			if(mHasGTIndex)
				mainWriteSession.indDb->append(ts,mainWriteSession.fbDb->blocksCount()-1);
			QScopedPointer<SensorValue> valWritten(hlp->unpackSensorValue(mStoredValuesType,data));
			if(valWritten.data())
				emit newValueWritten(valWritten.data());
			return true;
		}
		return false;
	}
	else
	{
		if(mainWriteSession.cbDb->writeBlock(data))
		{
			if(mHasGTIndex)
				mainWriteSession.indDb->append(ts,mainWriteSession.cbDb->blocksCount()-1);
			QScopedPointer<SensorValue> valWritten(hlp->unpackSensorValue(mStoredValuesType,data));
			if(valWritten.data())
				emit newValueWritten(valWritten.data());
			return true;
		}
		return false;
	}
}

bool SessionStorage::createAsFixedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	QDir dir(fsStorageHelper->dbPath());
	dir.mkdir("sessions");
	if(!dir.cd("sessions"))
		return false;
	if(!dir.cdUp())
		return false;
	mHasGTIndex=gtIndex&&(mStoredValuesType.tsType==SensorDef::GLOBAL_TIME);
	fsStorageHelper->settings()->setValue("db_type","fixed_blocks");
	fsStorageHelper->settings()->setValue("gt_index",mHasGTIndex?"1":"0");
	fsStorageHelper->settings()->sync();
	dbType=FIXED_BLOCKS;
	opened=true;
	return true;
}

bool SessionStorage::createAsChainedBlocksDb(bool gtIndex)
{
	if(opened)
		return false;
	QDir dir(fsStorageHelper->dbPath());
	dir.mkdir("sessions");
	if(!dir.cd("sessions"))
		return false;
	if(!dir.cdUp())
		return false;
	mHasGTIndex=gtIndex&&(mStoredValuesType.tsType==SensorDef::GLOBAL_TIME);
	fsStorageHelper->settings()->setValue("db_type","chained_blocks");
	fsStorageHelper->settings()->setValue("gt_index",mHasGTIndex?"1":"0");
	fsStorageHelper->settings()->sync();
	dbType=CHAINED_BLOCKS;
	opened=true;
	return true;
}

bool SessionStorage::open()
{
	if(opened)
		return false;
	QString dbTypeStr=fsStorageHelper->settings()->value("db_type").toString();
	if(dbTypeStr=="fixed_blocks")
		dbType=FIXED_BLOCKS;
	else if(dbTypeStr=="chained_blocks")
		dbType=CHAINED_BLOCKS;
	else
		return false;
	mHasGTIndex=(fsStorageHelper->settings()->value("gt_index").toString()=="1");
	opened=true;
	return true;
}

void SessionStorage::close()
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

void SessionStorage::closeSessionAndDeleteDb(SessionStorage::Session &d)
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

bool SessionStorage::listSessions(QList<QUuid> &ids,QByteArrayList &titles)
{
	if(!opened)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper->dbPath());
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

bool SessionStorage::createSession(const QByteArray &title,QUuid &id)
{
	if(!opened)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper->dbPath());
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
		DBDriverFixedBlocks tmpDrv;
		created=tmpDrv.create(sessionsDir.absolutePath()+"/"+idStr+"/data.db",
			DBDriverHelpers::sizesForFixedBlocksDb(mStoredValuesType));
	}
	else
	{
		DBDriverChainedBlocks tmpDrv;
		created=tmpDrv.create(sessionsDir.absolutePath()+"/"+idStr+"/data.db");
	}
	if(mHasGTIndex)
	{
		DBDriverGTimeIndex indDb;
		created=created&&indDb.create(sessionsDir.absolutePath()+"/"+idStr+"/index.db");
	}
	if(!created)
	{
		file.remove();
		sessionsDir.rmdir(idStr);
	}
	return created;
}

bool SessionStorage::openMainWriteSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!mainWriteSessionId.isNull())
		return false;
	QDir sessionDir=QDir(fsStorageHelper->dbPath());
	if(!sessionDir.cd("sessions"))
		return false;
	if(!sessionDir.cd(sessionId.toString()))
		return false;
	mainWriteSession.indDb=new DBDriverGTimeIndex;
	if(mHasGTIndex&&!mainWriteSession.indDb->open(sessionDir.absolutePath()+"/index.db"))
	{
		delete mainWriteSession.indDb;
		return false;
	}
	if(dbType==FIXED_BLOCKS)
	{
		mainWriteSession.fbDb=new DBDriverFixedBlocks;
		if(!mainWriteSession.fbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete mainWriteSession.fbDb;
			delete mainWriteSession.indDb;
			return false;
		}
	}
	else
	{
		mainWriteSession.cbDb=new DBDriverChainedBlocks;
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

bool SessionStorage::openSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(mainWriteSessionId==sessionId||sessions.contains(sessionId))
		return true;
	QDir sessionDir=QDir(fsStorageHelper->dbPath());
	if(!sessionDir.cd("sessions"))
		return false;
	if(!sessionDir.cd(sessionId.toString()))
		return false;
	Session s;
	s.indDb=new DBDriverGTimeIndex;
	if(mHasGTIndex&&!s.indDb->open(sessionDir.absolutePath()+"/index.db"))
	{
		delete s.indDb;
		return false;
	}
	if(dbType==FIXED_BLOCKS)
	{
		s.fbDb=new DBDriverFixedBlocks;
		if(!s.fbDb->open(sessionDir.absolutePath()+"/data.db"))
		{
			delete s.fbDb;
			delete s.indDb;
			return false;
		}
	}
	else
	{
		s.cbDb=new DBDriverChainedBlocks;
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

bool SessionStorage::closeMainWriteSession()
{
	if(mainWriteSessionId.isNull())
		return true;
	closeSessionAndDeleteDb(mainWriteSession);
	mainWriteSessionId=QUuid();
	return true;
}

bool SessionStorage::closeSession(const QUuid &sessionId)
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

bool SessionStorage::removeSession(const QUuid &sessionId)
{
	if(!opened||sessionId.isNull())
		return false;
	if(sessions.contains(sessionId)||mainWriteSessionId==sessionId)
		return false;
	QDir sessionsDir=QDir(fsStorageHelper->dbPath());
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

bool SessionStorage::setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	QSettings attrs(fsStorageHelper->dbPath()+"/sessions/"+sessionId.toString()+"/metadata.ini",QSettings::IniFormat);
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

bool SessionStorage::removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)
{
	if(!opened||sessionId.isNull())
		return false;
	if(!sessions.contains(sessionId)&&mainWriteSessionId!=sessionId)
		return false;
	QSettings attrs(fsStorageHelper->dbPath()+"/sessions/"+sessionId.toString()+"/metadata.ini",QSettings::IniFormat);
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

bool SessionStorage::getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)
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

bool SessionStorage::listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)
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

quint64 SessionStorage::valuesCount(const QUuid &sessionId)
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

quint64 SessionStorage::findInGTIndex(const QUuid &sessionId,qint64 ts)
{
	if(!opened||!mHasGTIndex||sessionId.isNull())
		return 0;
	if(mainWriteSessionId==sessionId)
		return mainWriteSession.indDb->findIndex(ts);
	else if(sessions.contains(sessionId))
		return sessions[sessionId].indDb->findIndex(ts);
	else
		return 0;
}

SensorValue *SessionStorage::valueAt(const QUuid &sessionId,quint64 index)
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
		return hlp->unpackSensorValue(mStoredValuesType,data);
	}
	else if(sessions.contains(sessionId))
	{
		Session &d=sessions[sessionId];
		QByteArray data;
		if(dbType==FIXED_BLOCKS&&!d.fbDb->readBlock(index,data))
			return 0;
		else if(dbType==CHAINED_BLOCKS&&!d.cbDb->readBlock((quint32)index,data))
			return 0;
		return hlp->unpackSensorValue(mStoredValuesType,data);
	}
	return 0;
}

bool SessionStorage::isSessionOpened(const QUuid &sessionId) const
{
	return opened&&(mainWriteSessionId==sessionId||sessions.contains(sessionId));
}

bool SessionStorage::isMainWriteSessionOpened() const
{
	return !mainWriteSessionId.isNull();
}

bool SessionStorage::isOpened() const
{
	return opened;
}

quint64 SessionStorage::valuesCount()
{
	return valuesCount(mainReadSessionId);
}

SensorValue* SessionStorage::valueAt(quint64 index)
{
	return valueAt(mainReadSessionId,index);
}

SensorValue *SessionStorage::lastValue()
{
	return valueAt(valuesCount()-1);
}

QUuid SessionStorage::getMainWriteSessionId() const
{
	return mainWriteSessionId;
}

bool SessionStorage::setMainReadSessionId(const QUuid &id)
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


void SessionStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	fsStorageHelper->writeAttribute(str,var);
}

QByteArray SessionStorage::readAttribute(const QByteArray &str)
{
	return fsStorageHelper->readAttribute(str);
}

void SessionStorage::addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)
{
	fsStorageHelper->addDataExportConfig(serviceId,cfg);
}

bool SessionStorage::hasDataExportConfig(const QUuid &serviceId)
{
	return fsStorageHelper->hasDataExportConfig(serviceId);
}

ISensorStorage::DataExportConfig SessionStorage::getDataExportConfig(const QUuid &serviceId)
{
	return fsStorageHelper->getDataExportConfig(serviceId);
}

void SessionStorage::removeDataExportConfig(const QUuid &serviceId)
{
	fsStorageHelper->removeDataExportConfig(serviceId);
}

QList<QUuid> SessionStorage::allDataExportServices()
{
	return fsStorageHelper->allDataExportServices();
}

bool SessionStorage::values(quint64 index,quint64 count,quint64 step,
	VeryBigArray<SensorValue*> &vals)
{
	if(!opened||mainReadSessionId.isNull())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		SensorValue *v=valueAt(mainReadSessionId,index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}

bool SessionStorage::hasGTIndex()
{
	return opened&&mHasGTIndex;
}

quint64 SessionStorage::findInGTIndex(qint64 ts)
{
	if(!opened||!mHasGTIndex||mainReadSessionId.isNull()||!sessions.contains(mainReadSessionId))
		return 0;
	return sessions[mainReadSessionId].indDb->findIndex(ts);
}
