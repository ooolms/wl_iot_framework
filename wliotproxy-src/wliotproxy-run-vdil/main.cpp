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
#include "CmdArgParser.h"
#include "VDILProcessing/VDILProgramConfigDb.h"
#include "Engine.h"

using namespace WLIOT;
using namespace WLIOTClient;
using namespace WLIOTVDIL;

VDILProgramConfigDb *cfgDb=0;

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
	QObject::connect(&stdinFile,&QFile::readyRead,&app,&QCoreApplication::quit);

	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly))
		return __LINE__;
	QByteArray data=file.readAll();
	file.close();
	//TODO plugins
	BlocksXmlParserFactory bxpf;
	BlocksFactory bf;
	QScopedPointer<Program> p(ProgramXmlParser::fromXml(&bxpf,&bf,data,false));
	if(p.isNull())
		return __LINE__;
	cfgDb=new VDILProgramConfigDb(filePath+".cfg",programId.toUtf8());
	cfgDb->cleanup(p.data());
	for(auto i=cfgDb->configOptions().begin();i!=cfgDb->configOptions().end();++i)
		p->setConfigOptionValue(i.key(),i.value());
	for(auto i=cfgDb->timers().begin();i!=cfgDb->timers().end();++i)
	{
		TimerBlock *b=p->timerBlocks().value(i.key());
		if(!b)continue;
		b->setConfig(i.value(),true);
	}

	ServerInstance srv;
	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.connection()->authenticateLocalFromRoot(user))
		return __LINE__;
	QObject::connect(srv.connection(),&ServerConnection::disconnected,&app,&QCoreApplication::quit);

	Engine e(&srv,p.data());
	return app.exec();
}
