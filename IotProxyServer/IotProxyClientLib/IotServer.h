#ifndef IOTSERVER_H
#define IOTSERVER_H

#include <QObject>
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServer
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServer(QObject *parent=nullptr);
	IotServerConnection* connection();
	IotServerCommands* commands();

private:
	IotServerConnection *conn;
	IotServerCommands *mCommands;
};

#endif // IOTSERVER_H
