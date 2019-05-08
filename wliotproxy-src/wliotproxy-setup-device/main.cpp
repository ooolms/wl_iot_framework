#include <QCoreApplication>
#include <QDebug>
#include "CmdArgParser.h"
#include "wliot/devices/SerialDevice.h"
#include "wliot/devices/CommandCall.h"
#include <iostream>

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	if(parser.args.count()<3||parser.vars.contains("help"))
	{
		std::cout<<"Usage:\nwliotproxy-setup-device <ttyPort> <newId> <newName>\n\n";
		std::cout<<"Example:\nwliotproxy-setup-device ttyACM0 0947ca44cf1c4b17a307574d75da574b some_device_1\n";
		return 0;
	}
	SerialDevice dev(parser.args[0]);
	if(dev.identify()==RealDevice::FAILED)
	{
		std::cerr<<"Not a valid device";
		return 1;
	}
	if(!dev.execCommand("#setup",QByteArrayList()<<parser.args[1].toUtf8()<<parser.args[2].toUtf8())->wait())
		return 1;
	return 0;
}
