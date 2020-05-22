/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "wliot/client/ServerCommandCall.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QCoreApplication>
#include <QDebug>

using namespace WLIOT;
using namespace WLIOTClient;

ServerCommandCall::ServerCommandCall(ServerConnection *conn,CmDataCallback onCmData,
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
	connect(&tmr,&QTimer::timeout,this,&ServerCommandCall::onTimeout);
	connect(conn,&ServerConnection::disconnected,this,&ServerCommandCall::onDisconnected,Qt::QueuedConnection);
	connect(conn,&ServerConnection::funcCallReplyMsg,this,&ServerCommandCall::onMessage,Qt::QueuedConnection);
}

void ServerCommandCall::call()
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

bool ServerCommandCall::ok()
{
	return mOk;
}

const QByteArrayList &ServerCommandCall::returnValue()
{
	return retVal;
}

void ServerCommandCall::onMessage(const Message &m)
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

void ServerCommandCall::onDisconnected()
{
	QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
	retVal.append("server disconnected");
}

void ServerCommandCall::onTimeout()
{
	if(done)return;
	QMetaObject::invokeMethod(this,"setDone",Qt::QueuedConnection);
	retVal.append("timeout");
}

void ServerCommandCall::setDone()
{
	done=true;
}
