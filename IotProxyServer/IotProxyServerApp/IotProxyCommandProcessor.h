#ifndef IOTPROXYCOMMANDPROCESSOR_H
#define IOTPROXYCOMMANDPROCESSOR_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "Commands/ICommand.h"
#include <QObject>
#include <QMap>

class IotProxyCommandProcessor
	:public QObject
{
	Q_OBJECT
public:
	explicit IotProxyCommandProcessor(ARpcOutsideDevice *d,QObject *parent=0);

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	ARpcOutsideDevice *dev;
	QMap<QString,ICommand*> commandProcs;
};

#endif // IOTPROXYCOMMANDPROCESSOR_H
