#include <QCoreApplication>
#include "IotClientCommandArgsParser.h"

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	IotClientCommandArgsParser parser(argc,argv);
	if(!parser.getCommandStatus())return 1;
	return app.exec();
}
