#ifndef IOTSERVERJSSCRIPTS_H
#define IOTSERVERJSSCRIPTS_H

#include "IotServerIJSScriptsManager.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerJSScripts
	:public IotServerIJSScriptsManager
{
	Q_OBJECT
public:
	explicit IotServerJSScripts(IotServerConnection *conn,IotServerCommands *cmds);
	virtual QByteArrayList scripts()override;
	virtual bool get(const QByteArray &scriptName,QByteArray &text)override;
	virtual bool isWorking(const QByteArray &scriptName)override;
	virtual bool setText(const QByteArray &scriptName,const QByteArray &text)override;
	virtual bool remove(const QByteArray &scriptName)override;
	virtual void start(const QByteArray &scriptName)override;
	virtual void stop(const QByteArray &scriptName)override;
	virtual void restart(const QByteArray &scriptName)override;
	bool reloadScripts();

private slots:
	void onConnected();
	void onDisconnected();

private:
	IotServerConnection *srvConn;
	IotServerCommands *srvCmds;
	QMap<QByteArray,bool> scriptsMap;
	bool ready;
};

#endif // IOTSERVERJSSCRIPTS_H
