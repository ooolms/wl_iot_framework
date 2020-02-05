#include "Device.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QScriptEngine>
#include <QScriptValue>
#include <QDomDocument>
#include <QDomElement>

//static const QUuid devId("{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}");
//static const char *devIdStr="{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}";
//static const QByteArray devName="test_vdev";

const QByteArray Device::jsScriptTemplate=QByteArray(
"//use global \"args\" variable to view args\n"
"var r={};\n"
"r[\"ok\"]=true;\n"
"r[\"value\"]=[\"some\",\"return\",\"values\"];\n"
"return r;"
);

static QScriptValue byteArrayListToJSArray(const QByteArrayList &list,QScriptEngine &eng)
{
	QScriptValue val=eng.newArray();
	for(int i=0;i<list.count();++i)
		val.setProperty(i,eng.toScriptValue(QString::fromUtf8(list[i])));
	return val;
}

void stateMapToXml(QDomDocument &doc,QDomElement &parentElem,const DeviceStateMap &map)
{
	for(auto i=map.commands.begin();i!=map.commands.end();++i)
	{
		QDomElement cmdElem=doc.createElement("command");
		parentElem.appendChild(cmdElem);
		cmdElem.setAttribute("command",QString::fromUtf8(i.key()));
		auto &m=i.value();
		for(auto j=m.begin();j!=m.end();++j)
		{
			QDomElement paramElem=doc.createElement("param");
			cmdElem.appendChild(paramElem);
			paramElem.setAttribute("index",j.key());
			paramElem.setAttribute("value",QString::fromUtf8(j.value()));
		}
	}
	for(auto i=map.additionalParams.begin();i!=map.additionalParams.end();++i)
	{
		QDomElement additParamElem=doc.createElement("additional_param");
		parentElem.appendChild(additParamElem);
		additParamElem.setAttribute("name",QString::fromUtf8(i.key()));
		additParamElem.setAttribute("value",QString::fromUtf8(i.value()));
	}
}

void stateMapFromXml(QDomDocument &doc,QDomElement &parentElem,DeviceStateMap &map)
{
	map.commands.clear();
	map.additionalParams.clear();
	for(int i=0;i<parentElem.childNodes().count();++i)
	{
		QDomElement elem=parentElem.childNodes().at(i).toElement();
		if(elem.isNull())continue;
		if(elem.tagName()=="command")
		{
			QByteArray cmd=elem.attribute("command").toUtf8();
			QMap<int,QByteArray> params;
			for(int j=0;j<elem.childNodes().count();++j)
			{
				QDomElement paramElem=elem.childNodes().at(j).toElement();
				if(paramElem.isNull()||paramElem.nodeName()!="param")continue;
				params[paramElem.attribute("index").toInt()]=paramElem.attribute("value").toUtf8();
			}
			map.commands[cmd]=params;
		}
		else if(elem.tagName()=="additional_param")
			map.additionalParams[elem.attribute("name").toUtf8()]=elem.attribute("value").toUtf8();
	}
}

Device::Device(QObject *parent)
	:QObject(parent)
{
	bCastCli=new QUdpSocket(this);
	bCastCli->bind(QHostAddress::AnyIPv4,4081);
	socket=new QTcpSocket(this);
	syncTimer=new QTimer(this);
	syncTimer->setInterval(8000);
	working=false;
	answerSyncMsgs=true;
	disconnectOnSyncTimeout=true;
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

void Device::disconnectFromServer()
{
	if(socket->state()!=QAbstractSocket::ConnectedState)return;
	socket->disconnectFromHost();
}

void Device::onNewMessage(Message m)
{
	QByteArray mm=m.dump();
	mm.chop(1);
	if(!working)
	{
		emit dbgMessage("new message (not working): "+mm);
		return;
	}
	else emit dbgMessage("new message: "+mm);
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
		{
			writeMsg(Message("syncr"));
			emit infoMessage("sync message, answering");
		}
		else emit infoMessage("sync message, ignoring");
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
		emit infoMessage("command: "+cmd+"|"+m.args.join("|"));

		if(commands.contains(cmd))
		{
			CommandReactionConfig cfg=commands[cmd];
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
			if(cfg.act==CommandReaction::CMD_WAIT_MANUAL_ANSWER)
				emit commandForManualReaction(cmd,m.args,cfg);
			if(cfg.act==CommandReaction::CMD_ANSWER_OK||cfg.act==CommandReaction::CMD_ANSWER_ERR)
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
				if(cfg.act==CommandReaction::CMD_ANSWER_OK)
				{
					emit infoMessage("answering ok: "+cfg.retVal.join('|'));
					writeOk(cfg.retVal);
				}
				else
				{
					emit infoMessage("answering err: "+cfg.retVal.join('|'));
					writeErr(cfg.retVal);
				}
				applyToStateMap(cfg.stateChangeAfterAnswer,toCmd,toAddit);
			}
			else if(cfg.act==CommandReaction::CMD_JS_EXEC)
			{
				QScriptEngine eng;
				eng.globalObject().setProperty("args",byteArrayListToJSArray(m.args,eng));
				QScriptValue v=eng.evaluate(cfg.jsScript);
				if(eng.hasUncaughtException())
				{
					QString msg="js exception: "+QString::number(eng.uncaughtExceptionLineNumber())+": ";
					msg+=eng.uncaughtException().toString();
					msg+="\t"+eng.uncaughtExceptionBacktrace().join("\n\t");
					emit infoMessage(msg.toUtf8());
				}
				if(v.isBool())
				{
					if(v.toBool())
					{
						emit infoMessage("answering ok from js: "+cfg.retVal.join('|'));
						writeOk(QByteArrayList());
					}
					else
					{
						emit infoMessage("answering err from js: "+cfg.retVal.join('|'));
						writeErr(QByteArrayList());
					}
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
					{
						emit infoMessage("answering ok from js: "+cfg.retVal.join('|'));
						writeOk(retVal);
					}
					else
					{
						emit infoMessage("answering err from js: "+cfg.retVal.join('|'));
						writeErr(retVal);
					}
				}
			}
			else if(cfg.act==CommandReaction::DEV_STUCK)
			{
				emit warningMessage("device is stuck when processing command");
				setWorking(false);
			}
			else if(cfg.act==CommandReaction::DEV_RESET)
			{
				emit warningMessage("device is reset when processing command");
				QThread::sleep(1);
				state.state=startupState;
				parser.reset();
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
			else
			{
				emit warningMessage("unknown system command: "+cmd);
				writeErr(QByteArrayList()<<"bad system command");
			}
		}
		else
		{
			emit warningMessage("unknown command: "+cmd);
			writeErr(QByteArrayList()<<"unknown command");
		}
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
	QByteArray mm=m.dump();
	mm.chop(1);
	emit dbgMessage("send msg: "+mm);
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

void Device::save(const QString &path)
{
	QDomDocument doc;
	QDomElement root=doc.createElement("device");
	doc.appendChild(root);
	root.setAttribute("uuid",devId.toString());
	root.setAttribute("name",QString::fromUtf8(devName));

	QDomElement sensElem=doc.createElement("sensors");
	root.appendChild(sensElem);
	sensElem.appendChild(doc.createTextNode(QString::fromUtf8(sensorsStr)));

	QDomElement contrElem=doc.createElement("controls");
	root.appendChild(contrElem);
	contrElem.appendChild(doc.createTextNode(QString::fromUtf8(controlsStr)));

	QDomElement commandsElem=doc.createElement("command_reactions");
	root.appendChild(commandsElem);

	for(auto i=commands.begin();i!=commands.end();++i)
	{
		CommandReactionConfig &cfg=i.value();
		Message m("",cfg.retVal);
		QDomElement cmdElem=doc.createElement("command_reaction");
		commandsElem.appendChild(cmdElem);
		cmdElem.setAttribute("command",QString::fromUtf8(i.key()));
		cmdElem.setAttribute("action",cmdActToString(cfg.act));
		cmdElem.setAttribute("return",QString::fromUtf8(m.dump()));
		cmdElem.setAttribute("js_script",cfg.jsScript);
		QDomElement stateChangeBeforeElem=doc.createElement("state_change_before_answer");
		cmdElem.appendChild(stateChangeBeforeElem);
		stateMapToXml(doc,stateChangeBeforeElem,cfg.stateChangeBeforeAnswer);
		QDomElement stateChangeAfterElem=doc.createElement("state_change_after_answer");
		cmdElem.appendChild(stateChangeAfterElem);
		stateMapToXml(doc,stateChangeAfterElem,cfg.stateChangeAfterAnswer);
	}

	QDomElement startupStateElem=doc.createElement("startup_state");
	root.appendChild(startupStateElem);
	stateMapToXml(doc,startupStateElem,startupState);

	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(doc.toByteArray());
	file.close();
}

void Device::load(const QString &path)
{
	QDomDocument doc;
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))return;
	if(!doc.setContent(file.readAll()))return;
	file.close();
	QDomElement root=doc.firstChildElement("device");
	if(root.isNull())return;

	devId=QUuid();
	devName.clear();
	sensorsStr.clear();
	controlsStr.clear();
	commands.clear();
	startupState.commands.clear();
	startupState.additionalParams.clear();

	devId=QUuid(root.attribute("uuid"));
	devName=root.attribute("name").toUtf8();

	sensorsStr=root.firstChildElement("sensors").text().toUtf8();
	controlsStr=root.firstChildElement("controls").text().toUtf8();

	QDomElement commandsElem=root.firstChildElement("command_reactions");
	if(commandsElem.isNull())return;
	for(int i=0;i<commandsElem.childNodes().count();++i)
	{
		QDomElement cmdElem=commandsElem.childNodes().at(i).toElement();
		QDomElement scbaElem=cmdElem.firstChildElement("state_change_before_answer").toElement();
		QDomElement scaaElem=cmdElem.firstChildElement("state_change_after_answer").toElement();
		if(cmdElem.isNull()||cmdElem.nodeName()!="command_reaction"||
			scbaElem.isNull()||scaaElem.isNull())continue;
		QByteArray command=cmdElem.attribute("command").toUtf8();
		CommandReactionConfig cfg;
		Message m;
		StreamParser::tryParse(cmdElem.attribute("return").toUtf8(),m);
		cfg.act=cmdActFromString(cmdElem.attribute("action"));
		cfg.retVal=m.args;
		cfg.jsScript=cmdElem.attribute("js_script").toUtf8();
		stateMapFromXml(doc,scbaElem,cfg.stateChangeBeforeAnswer);
		stateMapFromXml(doc,scaaElem,cfg.stateChangeAfterAnswer);
		commands[command]=cfg;
	}

	QDomElement startupStateElem=root.firstChildElement("startup_state");
	stateMapFromXml(doc,startupStateElem,startupState);
}

void Device::sendSensorValue(const QByteArray &sensor,const QByteArrayList &value)
{
	writeMsg(Message("meas",QByteArrayList()<<sensor<<value));
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
	{
		emit warningMessage("sync timeout, disconnecting");
		socket->disconnectFromHost();
	}
	else emit warningMessage("sync timeout, ignoring");
}

void Device::onSocketConnected()
{
	if(working)
		syncTimer->start();
	emit infoMessage("connected to server");
	emit connected();
}

void Device::onSocketDisconnected()
{
	if(working)
		syncTimer->stop();
	emit infoMessage("disconnected from server");
	emit disconnected();
}
