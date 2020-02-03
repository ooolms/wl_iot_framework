#include <QApplication>
#include "MainWindow.h"

//TODO license headers
//TODO complex ui to generate and send sensor values (use sensors xml)
//TODO logs

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	MainWindow w;
	w.showMaximized();
	return app.exec();
}
