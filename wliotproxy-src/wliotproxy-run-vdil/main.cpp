#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <wliot/client/ServerInstance.h>
#include <VDIL/core/BlocksFactory.h>
#include <VDIL/core/Program.h>
#include <VDIL/core/ProgramVirtualDevice.h>
#include <VDIL/xml/BlocksXmlParserFactory.h>
#include <VDIL/xml/ProgramXmlParser.h>
#include <unistd.h>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Category.hh>
#include "CmdArgParser.h"
#include "VDILProcessing/VDILProgramConfigDb.h"
#include "EngineRun.h"
#include "MainServerConfig.h"

using namespace WLIOT;
using namespace WLIOTClient;
using namespace WLIOTVDIL;

log4cpp::RollingFileAppender *logApp=0;
log4cpp::Layout* logLay=0;
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
	}
}

void logInit(const char *fileName)
{
	logApp=new log4cpp::RollingFileAppender("FileAppender",fileName);
	logLay=new log4cpp::BasicLayout();
	logApp->setLayout(logLay);
	logMain=&log4cpp::Category::getInstance("Main");
	logMain->setAdditivity(false);
	logMain->setAppender(logApp);
	logMain->setPriority(log4cpp::Priority::DEBUG);
	qInstallMessageHandler(&myMessageOutput);
}

void logDestroy()
{
	log4cpp::Category::shutdown();
}

VDILProgramConfigDb *cfgDb=0;

//TODO после переделывания программ на папки, переместить логи в подпапку log

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	QString programId=parser.getVarSingle("id");
	QString filePath=parser.getVarSingle("exec");
	QByteArray user=parser.getVarSingle("user").toUtf8();
	if(!MainServerConfig::readConfig(parser))
		return __LINE__;
	if(programId.isEmpty()||user.isEmpty())
		return __LINE__;
	logInit((filePath+".log").toLocal8Bit());

	//stdio handle - close on any data
	QFile stdinFile;
	stdinFile.open(STDIN_FILENO,QIODevice::ReadOnly);
	QObject::connect(&stdinFile,&QFile::readyRead,&app,&QCoreApplication::quit);

	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly))
		return __LINE__;
	QByteArray data=file.readAll();
	file.close();
	ServerInstance srv;
	EngineRun engine(&srv);
	if(!engine.setup(data))
		return __LINE__;
	cfgDb=new VDILProgramConfigDb(filePath+".cfg",programId.toUtf8());
	cfgDb->cleanup(engine.program());
	for(auto i=cfgDb->configOptions().begin();i!=cfgDb->configOptions().end();++i)
		engine.program()->setConfigOptionValue(i.key(),i.value());
	for(auto i=cfgDb->timers().begin();i!=cfgDb->timers().end();++i)
	{
		TimerBlock *b=engine.program()->timerBlocks().value(i.key());
		if(!b)continue;
		b->setConfig(i.value(),true);
	}

	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.connection()->authenticateLocalFromRoot(user))
		return __LINE__;
	QObject::connect(srv.connection(),&ServerConnection::disconnected,&app,&QCoreApplication::quit);

	engine.start();
	int r=app.exec();
	logDestroy();
	return r;
}