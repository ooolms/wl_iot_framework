#ifndef IOTSERVERJSSCRIPTSCOMMANDS_H
#define IOTSERVERJSSCRIPTSCOMMANDS_H

#include <QObject>
#include "IotServerConnection.h"

class IotServerJSScriptsCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerJSScriptsCommands(IotServerConnection *conn);
	bool list(QByteArrayList &scripts,QList<bool> &states);
	bool get(const QByteArray &scriptName,QByteArray &text);
	bool remove(const QByteArray &scriptName);
	bool upload(const QByteArray &scriptName,const QByteArray &text);
	bool start(const QByteArray &scriptName);
	bool stop(const QByteArray &scriptName);
	bool restart(const QByteArray &scriptName);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERJSSCRIPTSCOMMANDS_H
