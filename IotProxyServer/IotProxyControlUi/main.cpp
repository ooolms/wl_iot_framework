#include <QApplication>
#include "ControlUiWidget.h"
#include <QDebug>

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	if(app.arguments().count()<2)return 1;
	if(app.arguments().contains("--help"))
	{
		qDebug()<<"Usage: "<<qApp->arguments()[0]<<" <device_id_or_name>";
		return 0;
	}
	QString devIdOrName=app.arguments()[1];
	ControlUiWidget w(devIdOrName.toUtf8());
	if(!w.prepare())return 1;
	w.show();
	return app.exec();
}

