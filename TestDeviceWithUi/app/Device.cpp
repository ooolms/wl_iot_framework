#include "Device.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptValue>

//static const QUuid devId("{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}");
//static const char *devIdStr="{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}";
//static const QByteArray devName="test_vdev";

Device::Device(QObject *parent)
	:QObject(parent)
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
	connect(&parser,&StreamParser::newMessage,this,&Device::onNewMessage);
	connect(&srvReadyParser,&StreamParser::newMessage,this,&Device::onNewSrvReadyMessage);
}

bool Device::connectToServer(const QHostAddress &addr)
{
	if(socket->state()!=QAbstractSocket::UnconnectedState)
		return false;
	socket->connectToHost(addr,4081);
	return socket->waitForConnected();
}

void Device::onNewMessage(Message m)
{
	if(!working)return;
	if(m.title=="identify")
	{
		emit infoMessage("identification requested");
		writeMsg(Message("deviceinfo",QByteArrayList()<<devId.toByteArray()<<devName));
	}
	else if(m.title=="sync")
	{
		syncTimer->stop();
		syncTimer->start();
		if(answerSyncMsgs)
			writeMsg(Message("syncr"));
	}
	else if(m.title=="call")
	{
		callIdStr.clear();
		if(m.args.count()<2||m.args[0].size()==0||m.args[1].size()==0)
		{
			writeErr(QByteArrayList()<<"No command or call id");
			return;
		}
		callIdStr=m.args[0];
		QByteArray cmd=m.args[1];
		m.args.removeAt(0);
		m.args.removeAt(0);

		if(reactionCfg->commandReactions.contains(cmd))
		{
			DeviceReactionConfig::CommandCfg cfg=reactionCfg->commandReactions[cmd];
			for(unsigned long i=0;i<m.args.count();++i)
			{
				QByteArray replaceStr="${"+QByteArray::number((qulonglong)i)+"}";
				QByteArray &arg=m.args[i];
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
				emit commandForManualReaction(cmd,m.args,cfg);
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
			else if(cfg.act==DeviceReactionConfig::CMD_JS_EXEC)
			{
				QScriptEngine eng;
				QScriptValue v=eng.evaluate(cfg.jsScript);
				if(eng.hasUncaughtException())
				{
					QString msg="js exception: "+QString::number(eng.uncaughtExceptionLineNumber())+": ";
					msg+=eng.uncaughtException().toString();
					msg+="\t"+eng.uncaughtExceptionBacktrace().join("\n\t");
					emit infoMessage(msg);
				}
				if(v.isBool())
				{
					if(v.toBool())
						writeOk(QByteArrayList());
					else writeErr(QByteArrayList());
				}
				else if(v.isObject())
				{
					bool ok=v.property("ok").toBool();
					QByteArrayList retVal;
					QScriptValue arr=v.property("value");
					quint32 i=0;
					while(1)
					{
						QScriptValue vv=arr.property(i);
						if(!vv.isValid())break;
						retVal.append(vv.toString().toUtf8());
						++i;
					}
					if(ok)
						writeOk(retVal);
					else writeErr(retVal);
				}
			}
			else if(cfg.act==DeviceReactionConfig::DEV_STUCK)
				setWorking(false);
			else if(cfg.act==DeviceReactionConfig::DEV_RESET)
			{
				state.state=startupState;
				socket->write("0",1);
				socket->flush();
			}
		}
		else if(cmd.startsWith("#"))
		{
			if(cmd=="#sensors")
				writeOk(QByteArrayList()<<sensorsStr);
			else if(cmd=="#controls")
				writeOk(QByteArrayList()<<controlsStr);
			else if(cmd=="#state")
			{
				Message m;
				m.title="ok";
				state.dump(m);
				writeMsg(m);
			}
			else if(cmd=="#setup")
			{
				writeErr(QByteArrayList()<<"changing id is not supported");
			}
			else writeErr(QByteArrayList()<<"bad system command");
		}
		else writeErr(QByteArrayList()<<"unknown command");
	}
}

void Device::onNewSrvReadyMessage(Message m)
{
	if(m.title=="server_ready"&&m.args.count()>=2)
		emit serverFound(bCastSerderAddr,QUuid(m.args[0]),m.args[1]);
}

void Device::setWorking(bool w)
{
	if(working==w)return;
	working=w;
	if(working)
		state.state=startupState;
	emit workingChanged();
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

void Device::setSensors(const QByteArray &str)
{
	sensorsStr=str;
}

void Device::setControls(const QByteArray &str)
{
	controlsStr=str;
}

void Device::writeMsg(const Message &m)
{
	socket->write(m.dump());
	socket->flush();
}

void Device::setupUidAndName(const QUuid &id,const QByteArray &name)
{
	if(working)return;
	devId=id;
	devName=name;
}

QHostAddress Device::serverAddr()
{
	return socket->peerAddress();
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
	writeMsg(Message("ok",args));
}

void Device::writeErr(const QByteArrayList &args)
{
	writeMsg(Message("err",args));
}

void Device::onBCastCliReadyRead()
{
	while(bCastCli->hasPendingDatagrams())
	{
		QNetworkDatagram dg=bCastCli->receiveDatagram();
		bCastSerderAddr=dg.senderAddress();
		srvReadyParser.pushData(dg.data());
	}
}

void Device::onSocketReadyRead()
{
	parser.pushData(socket->readAll());
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
