#include "IotServerCommandCall.h"
#include "IotServerConnection.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QDebug>

IotServerCommandCall::IotServerCommandCall(IotServerConnection *conn,CmDataCallback onCmData,
	const QByteArray &cmd,const QByteArrayList &args,QObject *parent)
	:QObject(parent)
{
	mOnCmData=onCmData;
	static qint64 callIdNum=0;
	callId=QByteArray::number(++callIdNum);
	done=false;
	mOk=false;

	if(!conn->isConnected())
	{
		done=true;
		retVal.append("server is disconnected");
		return;
	}

	tmr.setSingleShot(true);
	tmr.setInterval(WLIOTProtocolDefs::syncWaitTime*4);


	//CRIT think about timers for a server
	connect(&tmr,&QTimer::timeout,this,&IotServerCommandCall::onTimeout);
	connect(conn,&IotServerConnection::disconnected,this,&IotServerCommandCall::onDisconnected,Qt::QueuedConnection);
	connect(conn,&IotServerConnection::funcCallReplyMsg,this,&IotServerCommandCall::onMessage);

	tmr.start();
	qDebug()<<"Command to server: "<<cmd<<":"<<callId<<":"<<args;
	conn->writeMsg(Message(cmd,QByteArrayList()<<callId<<args));
	if(!done)
		loop.exec();
	if(mOk)
		qDebug()<<"Command ok: "<<callId<<":"<<retVal;
	else qDebug()<<"Command err: "<<callId<<":"<<retVal;
}

bool IotServerCommandCall::ok()
{
	return mOk;
}

const QByteArrayList &IotServerCommandCall::returnValue()
{
	return retVal;
}

void IotServerCommandCall::onMessage(const Message &m)
{
	if(m.args.isEmpty())return;
	if(m.title==WLIOTProtocolDefs::funcAnswerOkMsg)
	{
		if(m.args[0]!=callId)return;
		done=true;
		mOk=true;
		retVal=m.args.mid(1);
		tmr.stop();
		loop.quit();
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerErrMsg)
	{
		if(m.args[0]!=callId)return;
		done=true;
		retVal=m.args.mid(1);
		tmr.stop();
		loop.quit();
	}
	else if(m.title==WLIOTServerProtocolDefs::srvCmdDataMsg)
	{
		if(m.args[0]!=callId||!mOnCmData)return;
		if(!mOnCmData(m.args.mid(1)))
		{
			done=true;
			tmr.stop();
			loop.quit();
		}
	}
}

void IotServerCommandCall::onDisconnected()
{
	done=true;
	retVal.append("server disconnected");
	loop.quit();
}

void IotServerCommandCall::onTimeout()
{
	if(done)return;
	done=true;
	retVal.append("timeout");
	loop.quit();
}
