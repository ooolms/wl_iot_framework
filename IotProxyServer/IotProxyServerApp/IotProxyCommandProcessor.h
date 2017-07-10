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
	virtual ~IotProxyCommandProcessor();

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	void addCommand(ICommand *c);

private:
	ARpcOutsideDevice *dev;
	QMap<QString,ICommand*> commandProcs;
	QList<ICommand*> commands;
};

#endif // IOTPROXYCOMMANDPROCESSOR_H
