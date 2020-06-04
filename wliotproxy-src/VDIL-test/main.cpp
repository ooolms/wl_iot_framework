#include <QApplication>
#include "VDIL/core/Program.h"
#include "VDIL/core/BlockOutput.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/editor/BlocksEditingFactory.h"
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/editor/Editor.h"
#include "StdQFile.h"
#include "MainWindow.h"
#include <QDebug>

void myMessageHandler(QtMsgType t,const QMessageLogContext &,const QString &s)
{
	QByteArray m=s.toUtf8();
	if(t==QtDebugMsg||t==QtInfoMsg)
	{
		StdQFile::stdoutFile()->write(m.constData(),m.size());
		StdQFile::stdoutFile()->write("\n",1);
		StdQFile::stdoutFile()->flush();
	}
	else
	{
		StdQFile::stderrFile()->write(m.constData(),m.size());
		StdQFile::stderrFile()->write("\n",1);
		StdQFile::stderrFile()->flush();
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
