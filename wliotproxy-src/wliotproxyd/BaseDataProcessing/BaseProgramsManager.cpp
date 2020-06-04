#include "BaseProgramsManager.h"
#include "../MainServerConfig.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include <QDir>

BaseProgramsManager::BaseProgramsManager(const QString &baseDirPath,QObject *parent)
	:QObject(parent)
{
	mBaseDirPath=QDir(baseDirPath).absolutePath()+"/";
}

void BaseProgramsManager::loadPrograms()
{
	QDir dir(mBaseDirPath);
	QStringList dirs=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &userDir:dirs)
	{
		bool ok=false;
		IdType uid=userDir.toLong(&ok);
		if(!ok)continue;
		if(!MainServerConfig::accessManager.hasUser(uid))return;
		QDir subdir(mBaseDirPath+userDir);
		QStringList files=subdir.entryList(QStringList()<<"*"+fileExtension(),QDir::Files);
		for(QString &f:files)
		{
			QByteArray prgId=f.toUtf8();
			prgId.chop(fileExtension().length());
			QString prgPath=subdir.absoluteFilePath(f);
			QFile file(prgPath);
			if(!file.open(QIODevice::ReadOnly))
				continue;
			QByteArray data=file.readAll();
			file.close();
			BaseProgramConfigDb *cfgDb=makeCfgDb(uid,prgId,prgPath);
			BaseProgramEngine *e=makeEngine(uid,data);
			if(!e)
			{
				delete cfgDb;
				continue;
			}
			programsMap[uid][prgId]=e;
			configsMap[uid][prgId]=cfgDb;
			e->setProgramName(cfgDb->programName());
			cfgDb->setup(e);
			if(cfgDb->isRunning())
				e->start();
		}
	}
}

BaseProgramsManager::~BaseProgramsManager()
{
	for(auto &l:programsMap)
		for(auto &e:l)
		{
			e->stop();
			delete e;
		}
	for(auto &l:configsMap)
		for(auto &e:l)
			delete e;
}

QByteArrayList BaseProgramsManager::programIds(IdType uid)
{
	return programsMap.value(uid).keys();
}

bool BaseProgramsManager::isWorking(IdType uid,const QByteArray &programId)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programId))
		return false;
	BaseProgramEngine *e=prgList[programId];
	return e->isRunning();
}

bool BaseProgramsManager::startStopProgram(IdType uid,const QByteArray &programId,bool start)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programId))
		return false;
	BaseProgramEngine *e=prgList[programId];
	if(start)
	{
		if(e->isRunning())
			return true;
		e->start();
		configsMap[uid][programId]->setRunning(true);
	}
	else if(e->isRunning())
	{
		e->stop();
		configsMap[uid][programId]->setRunning(false);
	}
	return true;
}

bool BaseProgramsManager::addProgram(IdType uid,QByteArray programName,const QByteArray &data,QByteArray &id)
{
	static const QString syms="1234567890qwertyuiopasdfghjklzxcvbnm";
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	QString userDir=mBaseDirPath+QString::fromUtf8(QByteArray::number(uid))+"/";
	QString fileName;
	do
	{
		fileName.clear();
		for(int i=0;i<16;++i)
			fileName.append(syms[qrand()%syms.length()]);
	}
	while(QFile::exists(userDir+fileName+fileExtension()));
	id=fileName.toUtf8();
	BaseProgramConfigDb *cfgDb=makeCfgDb(uid,id,userDir+fileName+fileExtension());
	BaseProgramEngine *e=makeEngine(uid,data);
	if(!e)
	{
		delete cfgDb;
		return false;
	}
	e->setProgramName(programName);
	cfgDb->setProgramName(programName);
	QFile file(userDir+fileName+fileExtension());
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(data)!=data.size())
	{
		file.close();
		return false;
	}
	file.close();
	programsMap[uid][id]=e;
	configsMap[uid][id]=cfgDb;
	cfgDb->setup(e);
	e->start();
	cfgDb->setRunning(true);
	return true;
}

bool BaseProgramsManager::getProgram(IdType uid,const QByteArray &programId,QByteArray &text)
{
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	QFile file(configsMap[uid][programId]->programPath());
	if(!file.open(QIODevice::ReadOnly))
		return false;
	text=file.readAll();
	file.close();
	return true;
}

bool BaseProgramsManager::removeProgram(IdType uid,const QByteArray &programId)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	BaseProgramEngine *e=programsMap[uid][programId];
	if(e->isRunning())
		e->stop();
	QFile file(configsMap[uid][programId]->programPath());
	if(!file.remove())
		return false;
	programsMap[uid].remove(programId);
	delete e;
	BaseProgramConfigDb *db=configsMap[uid].take(programId);
	db->rmDb();
	delete db;
	return true;
}

bool BaseProgramsManager::updateProgram(IdType uid,const QByteArray &programId,const QByteArray &data)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	BaseProgramEngine *e=programsMap[uid][programId];
	QByteArray oldData=e->data();
	bool running=e->isRunning();
	if(running)
		e->stop();
	if(!e->setData(data))
	{
		if(running)
			e->start();
		return false;
	}
	QFile file(configsMap[uid][programId]->programPath());
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(data)!=data.size())
	{
		file.close();
		return false;
	}
	file.close();
	configsMap[uid][programId]->setup(e);
	if(running)
		e->start();
	return true;
}

bool BaseProgramsManager::renameProgram(IdType uid,const QByteArray &programId,const QByteArray &name)
{
	if(name.isEmpty())return false;
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	BaseProgramEngine *e=programsMap[uid][programId];
	BaseProgramConfigDb *db=configsMap[uid].take(programId);
	if(!e||!db)return false;
	db->setProgramName(name);
	e->setProgramName(name);
	return true;
}

BaseProgramEngine* BaseProgramsManager::engine(IdType uid,const QByteArray &programId)
{
	return programsMap.value(uid).value(programId);
}

BaseProgramConfigDb* BaseProgramsManager::cfgDb(IdType uid,const QByteArray &programId)
{
	return configsMap.value(uid).value(programId);
}
