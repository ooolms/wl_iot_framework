#include <QCoreApplication>
#include "IotClientCommandArgsParser.h"

int main(int argc,char *argv[])
{
	Q_INIT_RESOURCE(help);
	QCoreApplication app(argc,argv);
	IotClientCommandArgsParser parser(argc,argv);
	if(parser.getCommandStatus()==IotClientCommandArgsParser::DONE)return 0;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::ERROR)return 1;
	return app.exec();
}
