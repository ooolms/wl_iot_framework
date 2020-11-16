#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Category.hh>
#include "ServerLogs.h"
#include <QString>
#include <QDir>
#include <QCoreApplication>
#include "StdQFile.h"
#include "ServerInstance.h"

log4cpp::Category *ServerLogs::mMainLogCat=0;
log4cpp::Category *ServerLogs::mDevicesLogCat=0;
bool ServerLogs::mDupLogToStdout=false;

static const QMessageLogContext nullLogContext;

void stdoutMessageHandler(QtMsgType type,const QMessageLogContext &,const QString &str)
{
	if(type==QtDebugMsg)
	{
		StdQFile::stdoutFile()->write(str.toUtf8());
		StdQFile::stdoutFile()->write("\n");
		StdQFile::stdoutFile()->flush();
	}
#if (QT_VERSION>=QT_VERSION_CHECK(5,5,0))
	else if(type==QtInfoMsg)
	{
		StdQFile::stdoutFile()->write(str.toUtf8());
		StdQFile::stdoutFile()->write("\n");
		StdQFile::stdoutFile()->flush();
	}
#endif
	else
	{
		StdQFile::stderrFile()->write(str.toUtf8());
		StdQFile::stderrFile()->write("\n");
		StdQFile::stderrFile()->flush();
	}
}

void logMsgHandler(QtMsgType type,const QMessageLogContext &,const QString &str)
{
	ServerLogs::logMain(type,str);
}

bool ServerLogs::setup(const QString &baseLogsDir,bool dupToStdout)
{
	mDupLogToStdout=dupToStdout;
	QDir dir(baseLogsDir);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	if(!dir.exists()||!dir.isReadable())return false;
	QString mainLogPath=dir.absoluteFilePath("main.log");
	QString devicesLogPath=dir.absoluteFilePath("devices.log");
	if(!checkLogFile(mainLogPath)||!checkLogFile(devicesLogPath))
		return false;
	mMainLogCat=mkLog("Main",mainLogPath.toUtf8().toStdString());
	mDevicesLogCat=mkLog("Devices",devicesLogPath.toUtf8().toStdString());
	qInstallMessageHandler(&logMsgHandler);
	return true;
}

void ServerLogs::terminate()
{
	log4cpp::Category::shutdown();
	mMainLogCat=0;
	mDevicesLogCat=0;
}

void ServerLogs::logMain(QtMsgType type,const QString &msg)
{
	if(mMainLogCat)
		log(type,msg.toUtf8(),mMainLogCat);
}

void ServerLogs::logDevices(QtMsgType type,const QString &msg)
{
	if(mDevicesLogCat)
		log(type,msg.toUtf8(),mDevicesLogCat);
}

void ServerLogs::logMain(QtMsgType type,const QByteArray &msg)
{
	if(mMainLogCat)
		log(type,msg,mMainLogCat);
}

void ServerLogs::logDevices(QtMsgType type,const QByteArray &msg)
{
	if(mDevicesLogCat)
		log(type,msg,mDevicesLogCat);
}

void ServerLogs::log(QtMsgType type,const QByteArray &msg,log4cpp::Category *cat)
{
	if(type==QtDebugMsg)
		cat->debug("%s",msg.constData());
#if (QT_VERSION>=QT_VERSION_CHECK(5,5,0))
	else if(type==QtInfoMsg)
		cat->info("%s",msg.constData());
#endif
	else if(type==QtWarningMsg)
		cat->warn("%s",msg.constData());
	else if(type==QtCriticalMsg)
		cat->crit("%s",msg.constData());
	else if(type==QtFatalMsg)
	{
		cat->fatal("%s",msg.constData());
		log4cpp::Category::shutdown();
		ServerInstance::inst().terminated=true;
		qApp->quit();
	}
	if(ServerLogs::dupLogToStdout())
		stdoutMessageHandler(type,nullLogContext,QString::fromUtf8(msg));
}

log4cpp::Category *ServerLogs::mkLog(const std::string &catName,const std::string &filePath)
{
	log4cpp::PatternLayout *lay=new log4cpp::PatternLayout();
	lay->setConversionPattern("%d{%Y.%m.%d %H:%M:%S.%l} %p: %m %n");
	log4cpp::RollingFileAppender *appender=new log4cpp::RollingFileAppender(
		"FileAppender",filePath,10*1024*1024,1,true,00600);
	appender->setLayout(lay);
	log4cpp::Category *retVal=&log4cpp::Category::getInstance(catName);
	retVal->setAdditivity(false);
	retVal->setAppender(appender);
	return retVal;
}

void ServerLogs::setLogPriority(log4cpp::Category *cat,QtMsgType minLogType)
{
	if(minLogType==QtDebugMsg)
		cat->setPriority(log4cpp::Priority::DEBUG);
#if (QT_VERSION>=QT_VERSION_CHECK(5,5,0))
	else if(minLogType==QtInfoMsg)
		cat->setPriority(log4cpp::Priority::INFO);
#endif
	else if(minLogType==QtWarningMsg)
		cat->setPriority(log4cpp::Priority::WARN);
	else if(minLogType==QtCriticalMsg)
		cat->setPriority(log4cpp::Priority::CRIT);
	else if(minLogType==QtFatalMsg)
		cat->setPriority(log4cpp::Priority::FATAL);
}

bool ServerLogs::checkLogFile(const QString &path)
{
	QFileInfo info(path);
	if(!info.exists())
	{
		QFile f(path);
		if(!f.open(QIODevice::WriteOnly))
			return false;
		f.close();
	}
	else if(!info.isWritable())return false;
	return true;
}
