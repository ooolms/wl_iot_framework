#include "IotServerJSScriptsCommands.h"

IotServerJSScriptsCommands::IotServerJSScriptsCommands(IotServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool IotServerJSScriptsCommands::list(QByteArrayList &scripts,QList<bool> &states)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("js_list",QByteArrayList(),scripts)||(retVal.count()&2)!=0)
		return false;
	scripts.clear();
	states.clear();
	for(int i=0;i<retVal.count()>>1;++i)
	{
		scripts.append(retVal[i<<1]);
		states.append(retVal[(i<<1)+1]=="1");
	}
	return true;
}

bool IotServerJSScriptsCommands::get(const QByteArray &scriptName, QByteArray &text)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("js_get",QByteArrayList()<<scriptName,retVal)||retVal.count()<1)
		return false;
	text.append(retVal[0]);
	return true;
}

bool IotServerJSScriptsCommands::remove(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_remove",QByteArrayList()<<scriptName);
}

bool IotServerJSScriptsCommands::upload(const QByteArray &scriptName,const QByteArray &text)
{
	return srvConn->execCommand("js_upload",QByteArrayList()<<scriptName<<text);
}

bool IotServerJSScriptsCommands::start(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_start",QByteArrayList()<<scriptName);
}

bool IotServerJSScriptsCommands::stop(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_stop",QByteArrayList()<<scriptName);
}

bool IotServerJSScriptsCommands::restart(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_restart",QByteArrayList()<<scriptName);
}
