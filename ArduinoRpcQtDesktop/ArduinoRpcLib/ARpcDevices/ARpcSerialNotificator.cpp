#include "ARpcSerialNotificator.h"

ARpcSerialNotificator::ARpcSerialNotificator()
{
	w.addPath("/dev");
	connect(&w,&QFileSystemWatcher::directoryChanged,this,&ARpcSerialNotificator::checkSerialPorts);
}

ARpcSerialNotificator::~ARpcSerialNotificator()
{
	w.removePath("dev");
}

ARpcSerialNotificator& ARpcSerialNotificator::inst()
{
	static ARpcSerialNotificator notificator;
	return notificator;
}
