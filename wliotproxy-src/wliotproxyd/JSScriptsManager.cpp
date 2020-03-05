#include "JSScriptsManager.h"
#include "MainServerConfig.h"
#include <QDir>

JSScriptsManager::JSScriptsManager(QObject *parent)
	:QObject(parent)
{
	QDir dir("/var/lib/wliotproxyd/js_data_processing");
	QStringList dirs=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &userDir:dirs)
	{
		bool ok=false;
		IdType uid=userDir.toLong(&ok);
		if(!ok)continue;
		if(!MainServerConfig::accessManager.hasUser(uid))return;
		QDir subdir("/var/lib/wliotproxyd/scripts_data_processing/"+userDir);
		QStringList files=subdir.entryList(QStringList()<<"*.js",QDir::Files);
		for(QString &f:files)
		{
			QFile file(dir.absoluteFilePath(f));
			if(!file.open(QIODevice::ReadOnly))
				continue;
			QString data=QString::fromUtf8(file.readAll());
			file.close();
			JSThread *t=new JSThread(data,dir.absoluteFilePath(f),uid,this);
			scriptsMap[uid][f]=t;
			t->setObjectName(f);
			t->setup();
		}
	}
}

JSScriptsManager::~JSScriptsManager()
{
	for(auto &l:scriptsMap)
		for(auto &t:l)
		{
			t->quit();
			t->wait(3000);
			t->terminate();
			delete t;
		}
}

QStringList JSScriptsManager::scripts(IdType uid)
{
	return scriptsMap.value(uid).keys();
}

bool JSScriptsManager::scriptIsWorking(IdType uid, const QString &scriptName)
{
	if(!scriptsMap.contains(uid))
		return false;
	JSThread *t=scriptsMap[uid].value(scriptName,nullptr);
	if(!t)return false;
	return t->isRunning();
}

bool JSScriptsManager::startStopScript(IdType uid,const QString &scriptName,bool start)
{
	if(!scriptsMap.contains(uid))
		return false;
	auto &scrList=scriptsMap[uid];
	if(!scrList.contains(scriptName))
		return false;
	JSThread *t=scrList[scriptName];
	if(start)
	{
		if(t->isRunning())
			return true;
		QFile file("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid)+"/"+scriptName);
		if(!file.open(QIODevice::ReadOnly))
			return false;
		QString data=QString::fromUtf8(file.readAll());
		file.close();
		t->updateScriptText(data);
		t->setup();
	}
	else if(t->isRunning())
	{
		t->quit();
		if(!t->wait(3000))
		{
			t->terminate();
			t->cleanupAfterTerminated();
		}
	}
	return true;
}

bool JSScriptsManager::addScript(IdType uid,QString scriptName,const QByteArray &text)
{
	if(scriptName.endsWith(".js",Qt::CaseInsensitive))
		scriptName.chop(3);
	scriptName.append(".js");
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(scriptsMap[uid].contains(scriptName))
		return false;
	QDir dir("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid));
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	QFile file("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid)+"/"+scriptName);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(text)!=text.size())
	{
		file.close();
		return false;
	}
	file.close();
	JSThread *t=new JSThread(text,file.fileName(),uid,this);
	scriptsMap[uid][scriptName]=t;
	t->setObjectName(scriptName);
	t->setup();
	return true;
}

bool JSScriptsManager::getScript(IdType uid,const QString &scriptName,QByteArray &text)
{
	if(!MainServerConfig::accessManager.hasUser(uid))
		return false;
	if(!scriptsMap[uid].contains(scriptName))
		return false;
	QFile file("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid)+"/"+scriptName);
	if(!file.open(QIODevice::ReadOnly))
		return false;
	text=file.readAll();
	file.close();
	return true;
}

bool JSScriptsManager::removeScript(IdType uid,const QString &scriptName)
{
	if(!scriptsMap.contains(uid))
		return false;
	if(!scriptsMap[uid].contains(scriptName))
		return false;
	JSThread *t=scriptsMap[uid][scriptName];
	if(t->isRunning())
	{
		t->quit();
		if(!t->wait(3000))
		{
			t->terminate();
			t->cleanupAfterTerminated();
		}
	}
	QFile file("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid)+"/"+scriptName);
	if(!file.remove())
		return false;
	scriptsMap[uid].remove(scriptName);
	delete t;
	return true;
}

bool JSScriptsManager::updateScript(IdType uid,const QString &scriptName,const QByteArray &text)
{
	if(!scriptsMap.contains(uid))
		return false;
	if(!scriptsMap[uid].contains(scriptName))
		return false;
	JSThread *t=scriptsMap[uid][scriptName];
	QFile file("/var/lib/wliotproxyd/js_data_processing/"+QByteArray::number(uid)+"/"+scriptName);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	if(file.write(text)!=text.size())
	{
		file.close();
		return false;
	}
	file.close();
	if(t->isRunning())
	{
		t->quit();
		if(!t->wait(3000))
		{
			t->terminate();
			t->cleanupAfterTerminated();
		}
	}
	t->updateScriptText(text);
	t->setup();
	return true;
}
