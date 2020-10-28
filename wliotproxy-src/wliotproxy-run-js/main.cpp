#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <wliot/client/ServerInstance.h>
#include <unistd.h>
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

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	QString programId=parser.getVarSingle("id");
	QString filePath=parser.getVarSingle("exec");
	QByteArray user=parser.getVarSingle("user").toUtf8();
	if(programId.isEmpty()||user.isEmpty())
		return __LINE__;

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
	srv.connection()->startConnectLocal();
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
