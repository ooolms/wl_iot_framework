#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <wliot/client/ServerInstance.h>
#include <unistd.h>
#include <signal.h>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Category.hh>
#include "CmdArgParser.h"
#include "JSProcessing/JSProgramConfigDb.h"
#include "JSDevicesList.h"
#include "JSStoragesDatabase.h"
#include "JSExtensions/JSConsole.h"
#include "JSExtensions/JSTimers.h"

using namespace WLIOT;
using namespace WLIOTClient;
using namespace WLIOTVDIL;

JSProgramConfigDb *cfgDb=0;

log4cpp::RollingFileAppender *logApp=0;
log4cpp::PatternLayout* logLay=0;
log4cpp::Category *logMain=0;

void myMessageOutput(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
	QByteArray localMsg = msg.toUtf8();
	if(type==QtDebugMsg)
		logMain->debug("%s",localMsg.constData());
	else if(type==QtInfoMsg)
		logMain->info("%s",localMsg.constData());
	else if(type==QtWarningMsg)
		logMain->warn("%s",localMsg.constData());
	else if(type==QtCriticalMsg)
		logMain->crit("%s",localMsg.constData());
	else if(type==QtFatalMsg)
	{
		logMain->fatal("%s",localMsg.constData());
		log4cpp::Category::shutdown();
		qApp->quit();
	}
}

void logInit(const char *fileName)
{
	logApp=new log4cpp::RollingFileAppender("FileAppender",fileName);
	logLay=new log4cpp::PatternLayout();
	logLay->setConversionPattern("%d{%Y.%m.%d %H:%M:%S.%l} %p: %m %n");
	logApp->setLayout(logLay);
	logMain=&log4cpp::Category::getInstance("Main");
	logMain->setAdditivity(false);
	logMain->setAppender(logApp);
	logMain->setPriority(log4cpp::Priority::DEBUG);
}

void logDestroy()
{
	log4cpp::Category::shutdown();
}

static void sigTermAction(int)
{
	qApp->quit();
}

static void catchSigTerm()
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&sigTermAction;
	sigset_t set;
	sigemptyset(&set);
	sa.sa_mask=set;
	sigaction(SIGTERM,&sa,0);
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	QString programId=parser.getVarSingle("id");
	QString filePath=parser.getVarSingle("exec");
	QByteArray user=parser.getVarSingle("user").toUtf8();
	if(programId.isEmpty()||user.isEmpty())
		return __LINE__;
	logInit((filePath+".log").toLocal8Bit());
	catchSigTerm();

	//stdio handle - close on any data
	QFile stdinFile;
	stdinFile.open(STDIN_FILENO,QIODevice::ReadOnly);
	stdinFile.readAll();
	QObject::connect(&stdinFile,&QFile::readyRead,&app,&QCoreApplication::quit);

	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly))
		return __LINE__;
	QByteArray data=file.readAll();
	file.close();

	ServerInstance srv;
	srv.connection()->prepareAuth(user,"");
	srv.connection()->startConnectStdio();
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.connection()->isReady())
		return __LINE__;
	QObject::connect(srv.connection(),&ServerConnection::disconnected,&app,&QCoreApplication::quit);

	QScriptEngine js;
	JSStoragesDatabase *jsDb=new JSStoragesDatabase(&js,&srv);
	JSConsole *cons=new JSConsole(&js);
	JSTimers *timers=new JSTimers(&js);
	JSDevicesList *devs=new JSDevicesList(&js,&srv);
//	connect(&js,&QScriptEngine::signalHandlerException,this,&JSThread::onScriptException,Qt::DirectConnection);
	//TODO signal handler
	js.evaluate("script=this;");
	QScriptValue gObj=js.globalObject();
	gObj.setProperty("SensorsDatabase",js.newQObject(jsDb),QScriptValue::ReadOnly);
	gObj.setProperty("Console",js.newQObject(cons),QScriptValue::ReadOnly);
	gObj.setProperty("Timer",js.newQObject(timers),QScriptValue::ReadOnly);
	gObj.setProperty("Devices",js.newQObject(devs),QScriptValue::ReadOnly);
	//TODO load libraries
	js.evaluate(QString::fromUtf8(data),programId);
	if(js.hasUncaughtException())
	{
		QScriptValue e=js.uncaughtException();
		qDebug()<<"Script exception: "<<programId<<":"<<e.property("lineNumber").toNumber()<<": "<<e.toString();
	}
	int r=app.exec();
	gObj.setProperty("SensorsDatabase",js.undefinedValue());
	gObj.setProperty("Console",js.undefinedValue());
	gObj.setProperty("Timer",js.undefinedValue());
	gObj.setProperty("Devices",js.undefinedValue());
	return r;
}
