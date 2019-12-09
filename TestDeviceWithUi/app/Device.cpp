#include "Device.h"
#include "ARpcArduStrHlp.h"
#include <QNetworkDatagram>
#include <QDebug>

static const ARpcUuid devId("{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}");
static const char *devIdStr="{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}";
static const QByteArray devName="test_vdev";

Device::Device(QObject *parent)
	:QObject(parent)
	,srvReady(200,this)
	,parser(300,this)
	,writer(this)
{
	bCastCli=new QUdpSocket(this);
	bCastCli->bind(QHostAddress::AnyIPv4,4081);
	socket=new QTcpSocket(this);
	syncTimer=new QTimer(this);
	syncTimer->setInterval(8000);
	working=true;
	answerSyncMsgs=true;
	disconnectOnSyncTimeout=true;
	reactionCfg=new DeviceReactionConfig(this);
	connect(bCastCli,&QUdpSocket::readyRead,this,&Device::onBCastCliReadyRead);
	connect(socket,&QTcpSocket::readyRead,this,&Device::onSocketReadyRead);
	connect(socket,&QTcpSocket::disconnected,this,&Device::onSocketDisconnected);
	connect(socket,&QTcpSocket::connected,this,&Device::onSocketConnected);
	connect(syncTimer,&QTimer::timeout,this,&Device::onSyncTimer);
}

bool Device::connectToServer(const QHostAddress &addr)
{
	if(socket->state()!=QAbstractSocket::UnconnectedState)
		return false;
	socket->connectToHost(addr,4081);
	return socket->waitForConnected();
}

void Device::processSrvReadyMsg(const ARpcUuid &serverId,const char *serverName)
{
	char idStr[39];
	serverId.toString(idStr);
	qDebug()<<"Server detected: "<<idStr<<", "<<serverName;
}

void Device::writeData(const char *data,unsigned long sz)
{
	socket->write(data,(qint64)sz);
}

void Device::writeStr(const char *str)
{
	socket->write(str);
}

void Device::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	if(!working)return;
	if(strcmp(msg,"identify")==0)
		writer.writeMsg("deviceinfo",devIdStr,devName);
	else if(strcmp(msg,"sync")==0)
	{
		syncTimer->stop();
		syncTimer->start();
		if(answerSyncMsgs)
			writer.writeMsg("syncr");
	}
	else if(strcmp(msg,"call")==0)
	{
		callIdStr.clear();
		if(argsCount<2||strlen(args[0])==0||strlen(args[1])==0)
		{
			writeErrNoEscape(F("No command or call id"));
			return;
		}
		callIdStr=QByteArray(args[0]);
		QByteArray cmd(args[1]);
		if(reactionCfg->commandReactions.contains(cmd))
		{
			if()
		}
		else if(cmd.startsWith("#"))
		{
			if(use_strcmp(args[1],PSTR("#sensors"))==0)
				writeOk(sensorsStr);
			else if(use_strcmp(args[1],PSTR("#controls"))==0)
				writeOk(controlsStr);
			else if(use_strcmp(args[1],PSTR("#state"))==0)
			{
				if(!writer.beginWriteMsg())return;
				writer.writeArgNoEscape(F("ok"));
				writer.writeArg(callIdStr,strlen(callIdStr));
				state.dump();
				writer.endWriteMsg();
			}
			else if(use_strcmp(args[1],PSTR("#setup"))==0)
			{
				writeErr("changing id is not supported");
			}
			else writeErrNoEscape(F("bad system command"));
		}
		else writeErrNoEscape("unknown command");
	}
}

void Device::writeOkNoEscape(const char *str)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("ok"));
	writer.writeArg(callIdStr.constData(),(unsigned long)callIdStr.size());
	writer.writeArgNoEscape(str);
	writer.endWriteMsg();
}

void Device::writeErrNoEscape(const char *str)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("err"));
	writer.writeArg(callIdStr,strlen(callIdStr));
	writer.writeArgNoEscape(str);
	writer.endWriteMsg();
}

void Device::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("ok"));
	writer.writeArg(callIdStr,strlen(callIdStr));
	if(arg1)
		writer.writeArg(arg1,strlen(arg1));
	if(arg2)
		writer.writeArg(arg2,strlen(arg2));
	if(arg3)
		writer.writeArg(arg3,strlen(arg3));
	if(arg4)
		writer.writeArg(arg4,strlen(arg4));
	writer.endWriteMsg();
}

void Device::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("err"));
	writer.writeArg(callIdStr,strlen(callIdStr));
	if(arg1)
		writer.writeArg(arg1,strlen(arg1));
	if(arg2)
		writer.writeArg(arg2,strlen(arg2));
	if(arg3)
		writer.writeArg(arg3,strlen(arg3));
	if(arg4)
		writer.writeArg(arg4,strlen(arg4));
	writer.endWriteMsg();
}

void Device::onBCastCliReadyRead()
{
	while(bCastCli->hasPendingDatagrams())
	{
		QNetworkDatagram dg=bCastCli->receiveDatagram();
		bCastSerderAddr=dg.senderAddress();
		QByteArray d=dg.data();
		srvReady.putData(d.constData(),(unsigned long)d.size());
	}
}

void Device::onSocketReadyRead()
{
	QByteArray d=socket->readAll();
	parser.putData(d.constData(),(quint32)d.size());
}

void Device::onSyncTimer()
{
	if(disconnectOnSyncTimeout)
		socket->disconnectFromHost();
}

void Device::onSocketConnected()
{
	syncTimer->start();
	emit connected();
}

void Device::onSocketDisconnected()
{
	syncTimer->stop();
	emit disconnected();
}
