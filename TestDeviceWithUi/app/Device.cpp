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
	,state(&writer)
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
	socket->flush();
}

void Device::writeStr(const char *str)
{
	socket->write(str);
	socket->flush();
}

void Device::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	QByteArray msg2(msg);
	QByteArrayList args2;
	for(unsigned char i=0;i<argsCount;++i)
		args2.append(QByteArray(args[i]));
	processMsg2(msg2,args2);
}

void Device::processMsg2(QByteArray &msg,QByteArrayList &args)
{
	if(!working)return;
	if(msg=="identify")
		writer.writeMsg("deviceinfo",devIdStr,devName);
	else if(msg=="sync")
	{
		syncTimer->stop();
		syncTimer->start();
		if(answerSyncMsgs)
			writer.writeMsg("syncr");
	}
	else if(msg=="call")
	{
		callIdStr.clear();
		if(args.count()<2||args[0].size()==0||args[1].size()==0)
		{
			writeErrNoEscape("No command or call id");
			return;
		}
		callIdStr=args[0];
		QByteArray cmd=args[1];
		args.removeAt(0);
		args.removeAt(0);

		if(reactionCfg->commandReactions.contains(cmd))
		{
			DeviceReactionConfig::CommandCfg cfg=reactionCfg->commandReactions[cmd];
			for(unsigned long i=0;i<args.count();++i)
			{
				QByteArray replaceStr="${"+QByteArray::number((qulonglong)i)+"}";
				QByteArray &arg=args[i];
				for(QByteArray &s:cfg.retVal)
					s.replace(replaceStr,arg);
				auto onCmd=[&replaceStr,&arg](const QByteArray &,int,QByteArray &value)
				{
					value.replace(replaceStr,arg);
				};
				auto onAddit=[&replaceStr,&arg](const QByteArray &,QByteArray &value)
				{
					value.replace(replaceStr,arg);
				};
				applyToStateMap(cfg.stateChangeBeforeAnswer,onCmd,onAddit);
				applyToStateMap(cfg.stateChangeAfterAnswer,onCmd,onAddit);
			}
			if(cfg.act==DeviceReactionConfig::CMD_WAIT_MANUAL_ANSWER)
				emit commandForManualReaction(cmd,args,cfg);
			if(cfg.act==DeviceReactionConfig::CMD_ANSWER_OK||cfg.act==DeviceReactionConfig::CMD_ANSWER_ERR)
			{
				auto toCmd=[this](const QByteArray &cmd,int paramIndex,QByteArray &value)
				{
					state.setCommandParamState(cmd,paramIndex,value);
				};
				auto toAddit=[this](const QByteArray &name,QByteArray &value)
				{
					state.setAdditionalParamState(name,value);
				};
				applyToStateMap(cfg.stateChangeBeforeAnswer,toCmd,toAddit);
				if(cfg.act==DeviceReactionConfig::CMD_ANSWER_OK)
					writeOk(cfg.retVal);
				else writeErr(cfg.retVal);
				applyToStateMap(cfg.stateChangeAfterAnswer,toCmd,toAddit);
			}
			else if(cfg.act==DeviceReactionConfig::DEV_STUCK)
				setWorking(false);
			else if(cfg.act==DeviceReactionConfig::DEV_RESET)
			{
				state.m=startupState;
				socket->write("0",1);
				socket->flush();
			}
		}
		else if(cmd.startsWith("#"))
		{
			if(use_strcmp(args[1],PSTR("#sensors"))==0)
				writeOk(QByteArrayList()<<sensorsStr);
			else if(use_strcmp(args[1],PSTR("#controls"))==0)
				writeOk(QByteArrayList()<<controlsStr);
			else if(use_strcmp(args[1],PSTR("#state"))==0)
			{
				if(!writer.beginWriteMsg())return;
				writer.writeArgNoEscape("ok");
				writer.writeArg(callIdStr,strlen(callIdStr));
				state.dump();
				writer.endWriteMsg();
			}
			else if(use_strcmp(args[1],PSTR("#setup"))==0)
			{
				writeErrNoEscape("changing id is not supported");
			}
			else writeErrNoEscape(F("bad system command"));
		}
		else writeErrNoEscape("unknown command");
	}
}

void Device::setWorking(bool w)
{
	if(working==w)return;
	working=w;
	if(working)
		state.m=startupState;
}

bool Device::isWorking()
{
	return working;
}

bool Device::isConnected()
{
	return socket->state()==QAbstractSocket::ConnectedState;
}

void Device::setStartupState(const DeviceStateMap &m)
{
	startupState=m;
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

void Device::applyToStateMap(DeviceStateMap &map,std::function<void(const QByteArray &,int,QByteArray &)> toCmdState,
	std::function<void(const QByteArray&,QByteArray&)> toAdditState)
{
	for(auto i=map.commands.begin();i!=map.commands.end();++i)
	{
		QMap<int,QByteArray> &cmdMap=i.value();
		for(auto j=cmdMap.begin();j!=cmdMap.end();++j)
			toCmdState(i.key(),j.key(),j.value());
	}
	for(auto i=map.additionalParams.begin();i!=map.additionalParams.end();++i)
		toAdditState(i.key(),i.value());
}

void Device::writeOk(const QByteArrayList &args)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("ok"));
	writer.writeArg(callIdStr,strlen(callIdStr));
	for(int i=0;i<args.count();++i)
		writer.writeArg(args[i].constData(),args[i].size());
	writer.endWriteMsg();
}

void Device::writeErr(const QByteArrayList &args)
{
	if(!writer.beginWriteMsg())return;
	writer.writeArgNoEscape(F("err"));
	writer.writeArg(callIdStr,strlen(callIdStr));
	for(int i=0;i<args.count();++i)
		writer.writeArg(args[i].constData(),args[i].size());
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
	if(working)
		syncTimer->start();
	emit connected();
}

void Device::onSocketDisconnected()
{
	if(working)
		syncTimer->stop();
	emit disconnected();
}
