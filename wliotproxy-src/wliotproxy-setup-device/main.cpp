#include <QCoreApplication>
#include <QDebug>
#include <QUuid>
#include "CmdArgParser.h"
#include "wliot/devices/SerialDeviceBackend.h"
#include "wliot/devices/StdHighLevelDeviceBackend.h"
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/CommandCall.h"
#include <iostream>

const char *helpStr=
"Usage:\nwliotproxy-setup-device <ttyPort> <newName> [--uuid=newId]\n\n"
"Example:\nwliotproxy-setup-device ttyACM0 some_device_1\n"
"or: \nwliotproxy-setup-device ttyACM0 some_device_1 --uuid=033960db747a49c8b3cdc5c82a18c294\nto specify device id";

using namespace WLIOT;

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	if(parser.args.count()<2||parser.vars.contains("help"))
	{
		std::cout<<helpStr<<std::endl;
		return 0;
	}
	QUuid uid=QUuid::createUuid();
	if(parser.vars.contains("uuid"))
		uid=QUuid(parser.getVarSingle("uuid"));
	if(uid.isNull())
	{
		std::cerr<<"ERROR: null id specified!"<<std::endl;
		return 1;
	}
	SerialDeviceBackend *be=new SerialDeviceBackend(parser.args[0]);
	RealDevice dev;
	dev.setBackend(new StdHighLevelDeviceBackend(be));
	be->tryOpen();
	if(!dev.isConnected())
	{
		std::cerr<<"ERROR: can't open tty device"<<std::endl;
		return 1;
	}
	if(dev.identify()==RealDevice::FAILED)
	{
		std::cerr<<"ERROR: not a compatible device"<<std::endl;
		return 1;
	}
	if(!dev.execCommand("#setup",QByteArrayList()<<uid.toByteArray()<<parser.args[1].toUtf8())->wait())
	{
		std::cerr<<"ERROR: setup failed on the device"<<std::endl;
		return 1;
	}
	std::cout<<"OK"<<std::endl;
	return 0;
}
