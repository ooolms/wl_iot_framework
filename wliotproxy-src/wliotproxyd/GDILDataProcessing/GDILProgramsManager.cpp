#include "GDILProgramsManager.h"
#include "../MainServerConfig.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include <QDir>

static const QString programsBaseDir=QString("/var/lib/wliotproxyd/gdil_data_processing/");

GDILProgramsManager::GDILProgramsManager(QObject *parent)
	:QObject(parent)
{
	QDir dir(programsBaseDir);
	QStringList dirs=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &userDir:dirs)
	{
		bool ok=false;
		IdType uid=userDir.toLong(&ok);
		if(!ok)continue;
		if(!MainServerConfig::accessManager.hasUser(uid))return;
		QDir subdir(programsBaseDir+userDir);
		QStringList files=subdir.entryList(QStringList()<<"*.gdil",QDir::Files);
		for(QString &f:files)
		{
			QString prgPath=subdir.absoluteFilePath(f);
			QFile file(prgPath);
			if(!file.open(QIODevice::ReadOnly))
				continue;
			QByteArray data=file.readAll();
			file.close();
			Program *p=ProgramXmlParser::fromXml(&blocksXmlFactory,&blocksFactory,data);
			if(!p)continue;
			GDILEngine *e=new GDILEngine(uid,&blocksFactory,&blocksXmlFactory,this);
			e->setProgram(p);
			programsMap[uid][f]=e;
			GDILProgramConfigDb *cfgDb=new GDILProgramConfigDb(prgPath);
			configsMap[uid][f]=cfgDb;
			cfgDb->load(p);
			if(cfgDb->isRunning())
				e->start();
		}
	}
}

GDILProgramsManager::~GDILProgramsManager()
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

QStringList GDILProgramsManager::programs(IdType uid)
{
	return programsMap.value(uid).keys();
}

bool GDILProgramsManager::isWorking(IdType uid, const QString &programName)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programName))
		return false;
	GDILEngine *e=prgList[programName];
	return e->isRunning();
}

bool GDILProgramsManager::startStopProgram(IdType uid,const QString &programName,bool start)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programName))
		return false;
	GDILEngine *e=prgList[programName];
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

bool GDILProgramsManager::addProgram(IdType uid,QString programName,const QByteArray &text)
{
	if(programName.endsWith(".gdil",Qt::CaseInsensitive))
		programName.chop(5);
	programName.append(".gdil");
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(programsMap[uid].contains(programName))
		return false;
	Program *p=ProgramXmlParser::fromXml(&blocksXmlFactory,&blocksFactory,text);
	if(!p)
		return false;
	QDir dir(programsBaseDir+QString::fromUtf8(QByteArray::number(uid)));
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	QFile file(programsBaseDir+QString::fromUtf8(QByteArray::number(uid))+"/"+programName);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(text)!=text.size())
	{
		file.close();
		return false;
	}
	file.close();
	GDILEngine *t=new GDILEngine(uid,&blocksFactory,&blocksXmlFactory,this);
	programsMap[uid][programName]=t;
	t->setProgram(p);
	GDILProgramConfigDb *cfgDb=new GDILProgramConfigDb(file.fileName());
	configsMap[uid][programName]=cfgDb;
	cfgDb->load(p);
	t->start();
	cfgDb->setRunning(true);
	return true;
}

bool GDILProgramsManager::getProgram(IdType uid,const QString &programName,QByteArray &text)
{
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	QFile file(programsBaseDir+QString::fromUtf8(QByteArray::number(uid))+"/"+programName);
	if(!file.open(QIODevice::ReadOnly))
		return false;
	text=file.readAll();
	file.close();
	return true;
}

bool GDILProgramsManager::removeProgram(IdType uid,const QString &programName)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	GDILEngine *e=programsMap[uid][programName];
	if(e->isRunning())
		e->stop();
	QFile file(programsBaseDir+QString::fromUtf8(QByteArray::number(uid))+"/"+programName);
	if(!file.remove())
		return false;
	programsMap[uid].remove(programName);
	delete e;
	GDILProgramConfigDb *db=configsMap[uid].take(programName);
	file.setFileName(db->dbPath());
	file.remove();
	delete db;
	return true;
}

bool GDILProgramsManager::updateProgram(IdType uid,const QString &programName,const QByteArray &text)
{
	if(!programsMap.contains(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	Program *p=ProgramXmlParser::fromXml(&blocksXmlFactory,&blocksFactory,text);
	if(!p)
		return false;
	GDILEngine *t=programsMap[uid][programName];
	QFile file(programsBaseDir+QString::fromUtf8(QByteArray::number(uid))+"/"+programName);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(text)!=text.size())
	{
		file.close();
		return false;
	}
	file.close();
	bool running=t->isRunning();
	if(running)
		t->stop();
	t->setProgram(p);
	configsMap[uid][programName]->cleanup(p);
	if(running)
		t->start();
	return true;
}

Program* GDILProgramsManager::program(IdType uid,const QString &programName)
{
	GDILEngine *p=programsMap.value(uid).value(programName);
	if(!p)
		return 0;
	return p->program();
}

GDILEngine* GDILProgramsManager::engine(IdType uid,const QString &programName)
{
	return programsMap.value(uid).value(programName);
}

GDILProgramConfigDb* GDILProgramsManager::cfgDb(IdType uid,const QString &programName)
{
	return configsMap.value(uid).value(programName);
}
