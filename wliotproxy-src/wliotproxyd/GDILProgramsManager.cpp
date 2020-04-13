#include "GDILProgramsManager.h"
#include "MainServerConfig.h"
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
			QFile file(subdir.absoluteFilePath(f));
			if(!file.open(QIODevice::ReadOnly))
				continue;
			QByteArray data=file.readAll();
			file.close();
			Program *p=ProgramXmlParser::fromXml(&blocksXmlFactory,&blocksFactory,data);
			if(!p)continue;
			Engine *e=new Engine(&helper,&cmdCb,&blocksFactory,&blocksXmlFactory,this);
			e->setProgram(p);
			programsMap[uid][f]=e;
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
	Engine *e=prgList[programName];
	return e->isRunning();
}

bool GDILProgramsManager::startStopProgram(IdType uid,const QString &programName,bool start)
{
	if(!programsMap.contains(uid))
		return false;
	auto &prgList=programsMap[uid];
	if(!prgList.contains(programName))
		return false;
	Engine *e=prgList[programName];
	if(start)
	{
		if(e->isRunning())
			return true;
		QFile file(programsBaseDir+QByteArray::number(uid)+"/"+programName);
		if(!file.open(QIODevice::ReadOnly))
			return false;
		QByteArray data=file.readAll();
		file.close();
		e->setProgram(data);
		e->start();
	}
	else if(e->isRunning())
		e->stop();
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
	QDir dir(programsBaseDir+QByteArray::number(uid));
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	QFile file(programsBaseDir+QByteArray::number(uid)+"/"+programName);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(text)!=text.size())
	{
		file.close();
		return false;
	}
	file.close();
	Engine *t=new Engine(&helper,&cmdCb,&blocksFactory,&blocksXmlFactory,this);
	programsMap[uid][programName]=t;
	t->setProgram(p);
	t->start();
	return true;
}

bool GDILProgramsManager::getProgram(IdType uid,const QString &programName,QByteArray &text)
{
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(!programsMap[uid].contains(programName))
		return false;
	QFile file(programsBaseDir+QByteArray::number(uid)+"/"+programName);
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
	Engine *e=programsMap[uid][programName];
	if(e->isRunning())
		e->stop();
	QFile file(programsBaseDir+QByteArray::number(uid)+"/"+programName);
	if(!file.remove())
		return false;
	programsMap[uid].remove(programName);
	delete e;
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
	Engine *t=programsMap[uid][programName];
	QFile file(programsBaseDir+QByteArray::number(uid)+"/"+programName);
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
	if(running)
		t->start();
	return true;
}
