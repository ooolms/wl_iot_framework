#include "IotServer.h"

IotServer::IotServer(QObject *parent)
	:QObject(parent)
{
	conn=new IotServerConnection(this);
	mCommands=new IotServerCommands(conn,this);
}

IotServerConnection* IotServer::connection()
{
	return conn;
}

IotServerCommands* IotServer::commands()
{
	return mCommands;
}
