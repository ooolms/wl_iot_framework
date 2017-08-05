#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "IotProxyCommandProcessor.h"
#include <QLocalSocket>
#include <QThread>

class ClientThread
	:public QThread
{
	Q_OBJECT
public:
	explicit ClientThread(QLocalSocket *sock,QObject *parent=0);
	virtual ~ClientThread();

private slots:
	void onStarted();

private:
	QLocalSocket *socket;
	ARpcOutsideDevice *dev;
	IotProxyCommandProcessor *proc;
};

#endif // CLIENTTHREAD_H
