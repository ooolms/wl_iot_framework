#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QLocalSocket>
#include "ARpcBase/ARpcOutsideDevice.h"
#include "IotProxyCommandProcessor.h"

class ClientThread
	:public QThread
{
	Q_OBJECT
public:
	explicit ClientThread(QLocalSocket *s,QObject *parent=0);
	virtual ~ClientThread();
	void setup();
	QLocalSocket* sock();

protected:
	virtual void run();

private:
	QLocalSocket *socket;
	ARpcOutsideDevice *dev;
	IotProxyCommandProcessor *proc;
};

#endif // CLIENTTHREAD_H
