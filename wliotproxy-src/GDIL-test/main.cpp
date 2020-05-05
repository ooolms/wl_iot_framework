#include <QApplication>
#include "GDIL/core/Program.h"
#include "GDIL/core/BlockOutput.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/editor/BlocksEditingFactory.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/editor/Editor.h"
#include "StdQFile.h"
#include "MainWindow.h"
#include <QDebug>

void myMessageHandler(QtMsgType t,const QMessageLogContext &,const QString &s)
{
	QByteArray m=s.toUtf8();
	if(t==QtDebugMsg||t==QtInfoMsg)
	{
		StdQFile::stdout()->write(m.constData(),m.size());
		StdQFile::stdout()->write("\n",1);
		StdQFile::stdout()->flush();
	}
	else
	{
		StdQFile::stderr()->write(m.constData(),m.size());
		StdQFile::stderr()->write("\n",1);
		StdQFile::stderr()->flush();
	}
}

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	qInstallMessageHandler(&myMessageHandler);
	MainWindow w;
	w.showMaximized();
	return app.exec();
}
