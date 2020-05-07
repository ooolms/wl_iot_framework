#include "IotServerJSScripts.h"

IotServerJSScripts::IotServerJSScripts(IotServerConnection *conn,IotServerCommands *cmds)
	:IotServerIJSScriptsManager(conn)
{
	srvConn=conn;
	srvCmds=cmds;
	ready=false;
	connect(conn,&IotServerConnection::connected,this,&IotServerJSScripts::onConnected);
	connect(conn,&IotServerConnection::disconnected,this,&IotServerJSScripts::onDisconnected);
	if(conn->isConnected())
		onConnected();
}


QByteArrayList IotServerJSScripts::scripts()
{
	return scriptsMap.keys();
}

bool IotServerJSScripts::get(const QByteArray &scriptName,QByteArray &text)
{
	if(!ready||!scriptsMap.contains(scriptName))
		return false;
	return srvCmds->jsScriptsCommands()->get(scriptName,text);
}

bool IotServerJSScripts::isWorking(const QByteArray &scriptName)
{
	return scriptsMap.value(scriptName,false);
}

bool IotServerJSScripts::setText(const QByteArray &scriptName,const QByteArray &text)
{
	if(!ready)
		return false;
	if(!srvCmds->jsScriptsCommands()->upload(scriptName,text))
		return false;
	onConnected();
	return true;
}

bool IotServerJSScripts::remove(const QByteArray &scriptName)
{
	if(!ready)
		return false;
	if(!srvCmds->jsScriptsCommands()->remove(scriptName))
		return false;
	onConnected();
	return true;
}

void IotServerJSScripts::start(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->start(scriptName))
		return;
	scriptsMap[scriptName]=true;
}

void IotServerJSScripts::stop(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->stop(scriptName))
		return;
	scriptsMap[scriptName]=false;
}

void IotServerJSScripts::restart(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->restart(scriptName))
		return;
	scriptsMap[scriptName]=true;
}

bool IotServerJSScripts::reloadScripts()
{
	if(!ready)return false;
	onConnected();
	return ready;
}

void IotServerJSScripts::onConnected()
{
	scriptsMap.clear();
	QByteArrayList n;
	QList<bool> s;
	ready=srvCmds->jsScriptsCommands()->list(n,s);
	if(!ready)return;
	for(int i=0;i<n.count();++i)
		scriptsMap[n[i]]=s[i];
}

void IotServerJSScripts::onDisconnected()
{
	ready=false;
	scriptsMap.clear();
}
