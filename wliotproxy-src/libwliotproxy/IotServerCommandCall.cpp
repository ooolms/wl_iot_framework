#include "IotServerCommandCall.h"
#include "IotServerConnection.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QCoreApplication>
#include <QDebug>

IotServerCommandCall::IotServerCommandCall(IotServerConnection *conn,CmDataCallback onCmData,
	const QByteArray &cmd,const QByteArrayList &args,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
	mCmd=cmd;
	mArgs=args;
	mOnCmData=onCmData;
	static qint64 callIdNum=0;
	callId=QByteArray::number(++callIdNum);
	done=false;
	mOk=false;

	tmr.setSingleShot(true);
	tmr.setInterval(WLIOTProtocolDefs::syncWaitTime*4);

	//CRIT think about timers for a server
	connect(&tmr,&QTimer::timeout,this,&IotServerCommandCall::onTimeout);
	connect(conn,&IotServerConnection::disconnected,this,&IotServerCommandCall::onDisconnected,Qt::QueuedConnection);
	connect(conn,&IotServerConnection::funcCallReplyMsg,this,&IotServerCommandCall::onMessage,Qt::QueuedConnection);
}

void IotServerCommandCall::call()
{
	if(!srvConn->isConnected())
	{
		mOk=false;
		done=true;
		retVal.append("disconnected");
		return;
	}
	tmr.start();
	qDebug()<<"Command to server: "<<mCmd<<":"<<callId<<":"<<mArgs;
	srvConn->writeMsg(Message(mCmd,QByteArrayList()<<callId<<mArgs));
	while(!done)
		qApp->processEvents(QEventLoop::WaitForMoreEvents);
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
		QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
		mOk=true;
		retVal=m.args.mid(1);
		tmr.stop();
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerErrMsg)
	{
		if(m.args[0]!=callId)return;
		QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
		retVal=m.args.mid(1);
		tmr.stop();
	}
	else if(m.title==WLIOTServerProtocolDefs::srvCmdDataMsg)
	{
		if(m.args[0]!=callId||!mOnCmData)return;
		if(!mOnCmData(m.args.mid(1)))
		{
			QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
			tmr.stop();
		}
	}
}

void IotServerCommandCall::onDisconnected()
{
	QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
	retVal.append("server disconnected");
}

void IotServerCommandCall::onTimeout()
{
	if(done)return;
	QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
	retVal.append("timeout");
}

void IotServerCommandCall::setDone()
{
	done=true;
}
