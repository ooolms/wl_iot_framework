#include "IotServerCommandCall.h"
#include "IotServerConnection.h"
#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcServerConfig.h"
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

	tmr.setSingleShot(true);
	tmr.setInterval(ARpcConfig::synccCallWaitTime*4);

	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(conn,&IotServerConnection::disconnected,&loop,&QEventLoop::quit);
	connect(conn,&IotServerConnection::funcCallReplyMsg,this,&IotServerCommandCall::onMessage);

	tmr.start();
//	qDebug()<<"CALL: "<<cmd<<"|"<<callId<<"|"<<args;
	conn->writeMsg(ARpcMessage(cmd,QByteArrayList()<<callId<<args));
	if(!done)loop.exec(QEventLoop::ExcludeUserInputEvents);
}

bool IotServerCommandCall::ok()
{
	return mOk;
}

const QByteArrayList &IotServerCommandCall::returnValue()
{
	return retVal;
}

void IotServerCommandCall::onMessage(const ARpcMessage &m)
{
	if(m.args.isEmpty())return;
	if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		if(m.args[0]!=callId)return;
		done=true;
		mOk=true;
		retVal=m.args.mid(1);
//		qDebug()<<"OK: "<<callId<<"|"<<retVal;
		tmr.stop();
		loop.quit();
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		if(m.args[0]!=callId)return;
		done=true;
		retVal=m.args.mid(1);
//		qDebug()<<"ERR: "<<callId<<"|"<<retVal;
		tmr.stop();
		loop.quit();
	}
	else if(m.title==ARpcServerConfig::srvCmdDataMsg)
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
