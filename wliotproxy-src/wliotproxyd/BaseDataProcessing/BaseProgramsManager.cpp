#include "BaseProgramsManager.h"
#include "../MainServerConfig.h"
#include "GDIL/xml/ProgramXmlParser.h"
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
			QString prgPath=subdir.absoluteFilePath(f);
			QFile file(prgPath);
			if(!file.open(QIODevice::ReadOnly))
				continue;
			QByteArray data=file.readAll();
			file.close();
			BaseProgramConfigDb *cfgDb=makeCfgDb(uid,prgPath);
			BaseProgramEngine *e=makeEngine(uid,data);
			if(!e)
			{
				delete cfgDb;
				continue;
			}
			programsMap[uid][cfgDb->programName()]=e;
			configsMap[uid][cfgDb->programName()]=cfgDb;
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

QByteArrayList BaseProgramsManager::programs(IdType uid)
{
	return programsMap.value(uid).keys();
}

bool BaseProgramsManager::isWorking(IdType uid,const QByteArray &programName)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programName))
		return false;
	BaseProgramEngine *e=prgList[programName];
	return e->isRunning();
}

bool BaseProgramsManager::startStopProgram(IdType uid,const QByteArray &programName,bool start)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programName))
		return false;
	BaseProgramEngine *e=prgList[programName];
	if(start)
	{
		if(e->isRunning())
			return true;
		e->start();
		configsMap[uid][programName]->setRunning(true);
	}
	else if(e->isRunning())
	{
		e->stop();
		configsMap[uid][programName]->setRunning(false);
	}
	return true;
}

bool BaseProgramsManager::addProgram(IdType uid,QByteArray programName,const QByteArray &data)
{
	static const QString syms="1234567890qwertyuiopasdfghjklzxcvbnm";
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(programsMap[uid].contains(programName))
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
	BaseProgramConfigDb *cfgDb=makeCfgDb(uid,userDir+fileName+fileExtension());
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
	programsMap[uid][programName]=e;
	configsMap[uid][programName]=cfgDb;
	cfgDb->setup(e);
	e->start();
	cfgDb->setRunning(true);
	return true;
}

bool BaseProgramsManager::getProgram(IdType uid,const QByteArray &programName,QByteArray &text)
{
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	QFile file(configsMap[uid][programName]->programPath());
	if(!file.open(QIODevice::ReadOnly))
		return false;
	text=file.readAll();
	file.close();
	return true;
}

bool BaseProgramsManager::removeProgram(IdType uid,const QByteArray &programName)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	BaseProgramEngine *e=programsMap[uid][programName];
	if(e->isRunning())
		e->stop();
	QFile file(configsMap[uid][programName]->programPath());
	if(!file.remove())
		return false;
	programsMap[uid].remove(programName);
	delete e;
	BaseProgramConfigDb *db=configsMap[uid].take(programName);
	db->rmDb();
	delete db;
	return true;
}

bool BaseProgramsManager::updateProgram(IdType uid,const QByteArray &programName,const QByteArray &data)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	BaseProgramEngine *e=programsMap[uid][programName];
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
	QFile file(configsMap[uid][programName]->programPath());
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(data)!=data.size())
	{
		file.close();
		return false;
	}
	file.close();
	configsMap[uid][programName]->setup(e);
	if(running)
		e->start();
	return true;
}

BaseProgramEngine* BaseProgramsManager::engine(IdType uid,const QByteArray &programName)
{
	return programsMap.value(uid).value(programName);
}

BaseProgramConfigDb* BaseProgramsManager::cfgDb(IdType uid,const QByteArray &programName)
{
	return configsMap.value(uid).value(programName);
}
