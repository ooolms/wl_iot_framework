#include "BaseProgramsManager.h"
#include "../MainServerConfig.h"
#include "../ServerInstance.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include <QDir>
#include <QDebug>

//CRIT переделать все на папки, не забыть про логи

static const char *uidPropName="uid";
static const char *prgIdPropName="prg_id";

BaseProgramsManager::BaseProgramsManager(const QString &baseDirPath,QObject *parent)
	:QObject(parent)
{
	mBaseDirPath=QDir(baseDirPath).absolutePath()+"/";
}

void BaseProgramsManager::loadPrograms()
{
	processRunPath=ServerInstance::inst().srvBinDir()+"/"+processName();
	QDir dir(mBaseDirPath);
	QStringList dirs=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &userDir:dirs)
	{
		bool ok=false;
		IdType uid=userDir.toLong(&ok);
		if(!ok)continue;
		if(!MainServerConfig::accessManager.hasUser(uid))continue;
		QDir subdir(mBaseDirPath+userDir);
		QStringList files=subdir.entryList(QStringList()<<"*"+fileExtension(),QDir::Files);
		QString userName=QString::fromUtf8(MainServerConfig::accessManager.userName(uid));
		if(userName.isEmpty())continue;
		for(QString f:files)
		{
			if(!f.endsWith(fileExtension()))continue;
			QString prgPath=subdir.absoluteFilePath(f);
			f.chop(fileExtension().length());
			QByteArray prgId=f.toUtf8();
			QProcess *proc=new QProcess;
			proc->setProperty(uidPropName,uid);
			proc->setProperty(prgIdPropName,prgId);
			QStringList runArgs;
			runArgs.append("--id="+QString::fromUtf8(prgId));
			runArgs.append("--exec="+prgPath);
			runArgs.append("--user="+userName);
			proc->setProgram(processRunPath);
			proc->setArguments(runArgs);
			BaseProgramConfigDb *cfgDb=makeCfgDb(uid,prgId,prgPath);
			CommandProcessor *cp=0;
			if(cfgDb->isRunning())
			{
				cp=new CommandProcessor(proc);
				proc->start();
				proc->waitForStarted();
			}
			programsMap[uid][prgId]={proc,cp};
			configsMap[uid][prgId]=cfgDb;
			connect(proc,static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),
				this,&BaseProgramsManager::onProcFinished);
			//connect(proc,&QProcess::readyReadStandardOutput,this,&BaseProgramsManager::onProcReadyRead);
			//connect(proc,&QProcess::readyReadStandardError,this,&BaseProgramsManager::onProcReadyRead);
			//TODO process errors
		}
	}
}

BaseProgramsManager::~BaseProgramsManager()
{
	for(auto &l:programsMap)
		for(auto &cliSet:l)
		{
			if(cliSet.cp)
			{
				delete cliSet.cp;
				cliSet.cp=0;
			}
			if(cliSet.proc->state()!=QProcess::NotRunning)
				stopProcess(cliSet.proc);
			delete cliSet.proc;
		}
	for(auto &l:configsMap)
		for(auto &e:l)
			delete e;
}

QByteArrayList BaseProgramsManager::programIds(IdType uid)
{
	return programsMap.value(uid).keys();
}

bool BaseProgramsManager::isRunning(IdType uid,const QByteArray &programId)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programId))
		return false;
	QProcess *proc=prgList[programId].proc;
	return proc->state()!=QProcess::NotRunning;
}

bool BaseProgramsManager::startStopProgram(IdType uid,const QByteArray &programId,bool start)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programId))
		return false;
	ClientSet &cli=prgList[programId];
	if(start)
	{
		if(cli.proc->state()!=QProcess::NotRunning)
			return true;
		CommandProcessor *cp=new CommandProcessor(cli.proc);
		cli.cp=cp;
		cli.proc->start();
		cli.proc->waitForStarted();
		configsMap[uid][programId]->setRunning(true);
	}
	else
	{
		if(cli.proc->state()!=QProcess::NotRunning)
			stopProcess(cli.proc);
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
	if(!QDir(userDir).exists())
		QDir(userDir).mkpath(".");
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
	QProcess *proc=new QProcess;
	proc->setProperty(uidPropName,uid);
	proc->setProperty(prgIdPropName,id);
	QStringList runArgs;
	runArgs.append("--id="+QString::fromUtf8(id));
	runArgs.append("--exec="+userDir+fileName+fileExtension());
	runArgs.append("--user="+QString::fromUtf8(MainServerConfig::accessManager.userName(uid)));
	runArgs.append("--config="+MainServerConfig::configPath);
	proc->setProgram(processRunPath);
	proc->setArguments(runArgs);
//	connect(proc,&QProcess::readyReadStandardOutput,this,&BaseProgramsManager::onProcReadyRead);
//	connect(proc,&QProcess::readyReadStandardError,this,&BaseProgramsManager::onProcReadyRead);
	CommandProcessor *cp=new CommandProcessor(proc);
	programsMap[uid][id]={proc,cp};
	configsMap[uid][id]=cfgDb;
	proc->start();
	proc->waitForStarted();
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
	QProcess *proc=programsMap[uid][programId].proc;
	if(proc->state()!=QProcess::NotRunning)
		stopProcess(proc);
	QFile file(configsMap[uid][programId]->programPath());
	if(!file.remove())
		return false;
	programsMap[uid].remove(programId);
	delete proc;
	BaseProgramConfigDb *db=configsMap[uid].take(programId);
	db->rmDb();
	delete db;
	//CRIT remove logs
	return true;
}

bool BaseProgramsManager::updateProgram(IdType uid,const QByteArray &programId,const QByteArray &data)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	ClientSet &cli=programsMap[uid][programId];
	bool running=(cli.proc->state()!=QProcess::NotRunning);
	if(running)
		stopProcess(cli.proc);
	QFile file(configsMap[uid][programId]->programPath());
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(data)!=data.size())
	{
		file.close();
		return false;
	}
	file.close();
	if(running)
	{
		CommandProcessor *cp=new CommandProcessor(cli.proc);
		cli.cp=cp;
		cli.proc->start();
		cli.proc->waitForStarted();
	}
	return true;
}

bool BaseProgramsManager::renameProgram(IdType uid,const QByteArray &programId,const QByteArray &name)
{
	if(name.isEmpty())return false;
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programId))
		return false;
	BaseProgramConfigDb *db=configsMap[uid][programId];
	if(!db)return false;
	db->setProgramName(name);
	return true;
}

BaseProgramConfigDb* BaseProgramsManager::cfgDb(IdType uid,const QByteArray &programId)
{
	return configsMap.value(uid).value(programId);
}

void BaseProgramsManager::onProcFinished()
{
	QProcess *proc=(QProcess*)sender();
	IdType uid=proc->property(uidPropName).value<IdType>();
	QByteArray prgId=proc->property(prgIdPropName).toByteArray();
	if(!programsMap[uid].contains(prgId))return;
	ClientSet &set=programsMap[uid][prgId];
	if(!set.cp)return;
	delete set.cp;
	set.cp=0;
}

/*void BaseProgramsManager::onProcReadyRead()
{
	QProcess *proc=(QProcess*)sender();
	QByteArray data=proc->readAllStandardOutput();
	if(!data.isEmpty())
		qDebug()<<"program"<<fileExtension()<<" output: "<<data;
	data=proc->readAllStandardError();
	if(!data.isEmpty())
		qDebug()<<"program"<<fileExtension()<<" error: "<<data;
}*/

void BaseProgramsManager::stopProcess(QProcess *proc)
{
	proc->terminate();
	proc->waitForFinished();
}
