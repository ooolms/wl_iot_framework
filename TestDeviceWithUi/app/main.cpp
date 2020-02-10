#include <QApplication>
#include "MainWindow.h"
#include "CmdArgParser.h"

//TODO license headers
//TODO complex ui to generate and send sensor values (use sensors xml)
//TODO logs

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	MainWindow w;
	if(parser.vars.contains("load"))
	{
		w.loadDev(parser.getVarSingle("load"));
		if(!w.dev->id().isNull())
			w.enableDev(true);
		if(parser.vars.contains("connect"))
			w.connectToServer(parser.getVarSingle("connect"));
	}
	w.showMaximized();
	return app.exec();
}
