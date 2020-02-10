#include "TestDevice.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QScriptValue>
#include <QDomDocument>
#include <QDomElement>
#include <QScriptValueIterator>

//static const QUuid devId("{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}");
//static const char *devIdStr="{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}";
//static const QByteArray devName="test_vdev";

const QByteArray TestDevice::jsScriptTemplate=QByteArray(
"//use global \"args\" variable to view args\n"
"var r={};\n"
"r[\"ok\"]=true;\n"
"r[\"value\"]=[\"some\",\"return\",\"values\"];\n"
"return r;"
);

static QScriptValue byteArrayListToJSArray(const QByteArrayList &list,QScriptEngine *eng)
{
	QScriptValue val=eng->newArray();
	for(int i=0;i<list.count();++i)
		val.setProperty(i,eng->toScriptValue(QString::fromUtf8(list[i])));
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

void stateMapFromXml(QDomElement &parentElem,DeviceStateMap &map)
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
			QMap<quint32,QByteArray> params;
			for(int j=0;j<elem.childNodes().count();++j)
			{
				QDomElement paramElem=elem.childNodes().at(j).toElement();
				if(paramElem.isNull()||paramElem.nodeName()!="param")continue;
				params[paramElem.attribute("index").toUInt()]=paramElem.attribute("value").toUtf8();
			}
			map.commands[cmd]=params;
		}
		else if(elem.tagName()=="additional_param")
			map.additionalParams[elem.attribute("name").toUtf8()]=elem.attribute("value").toUtf8();
	}
}

TestDevice::TestDevice(QObject *parent)
	:QObject(parent)
{
	scriptEng=new QScriptEngine;
	scriptEng->evaluate("script=this;");
	timers=new JSTimers(scriptEng,scriptEng);
	scriptEng->globalObject().setProperty("Timer",scriptEng->newQObject(timers),QScriptValue::ReadOnly);
	bCastCli=new QUdpSocket(this);
	bCastCli->bind(QHostAddress::AnyIPv4,4081);
	socket=new QTcpSocket(this);
	syncTimer=new QTimer(this);
	syncTimer->setInterval(8000);
	working=false;
	answerSyncMsgs=true;
	disconnectOnSyncTimeout=false;
	connect(bCastCli,&QUdpSocket::readyRead,this,&TestDevice::onBCastCliReadyRead);
	connect(socket,&QTcpSocket::readyRead,this,&TestDevice::onSocketReadyRead);
	connect(socket,&QTcpSocket::disconnected,this,&TestDevice::onSocketDisconnected);
	connect(socket,&QTcpSocket::connected,this,&TestDevice::onSocketConnected);
	connect(syncTimer,&QTimer::timeout,this,&TestDevice::onSyncTimer);
	connect(&parser,&StreamParser::newMessage,this,&TestDevice::onNewMessage);
	connect(&srvReadyParser,&StreamParser::newMessage,this,&TestDevice::onNewSrvReadyMessage);
}

TestDevice::~TestDevice()
{
	delete scriptEng;
}

bool TestDevice::connectToServer(const QHostAddress &addr)
{
	if(socket->state()!=QAbstractSocket::UnconnectedState)
		return false;
	socket->connectToHost(addr,4081);
	return socket->waitForConnected();
}

void TestDevice::disconnectFromServer()
{
	if(socket->state()!=QAbstractSocket::ConnectedState)return;
	socket->disconnectFromHost();
}

void TestDevice::onNewMessage(Message m)
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
			for(int i=0;i<m.args.count();++i)
			{
				QByteArray replaceStr="${"+QByteArray::number((qulonglong)i)+"}";
				QByteArray &arg=m.args[i];
				for(QByteArray &s:cfg.retVal)
					s.replace(replaceStr,arg);
				auto onCmd=[&replaceStr,&arg](const QByteArray &,quint32,QByteArray &value)
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

			//1-st step - process manual and js actions, they may change action to other (ok,err,stuck,reset)
			if(cfg.act==CommandReaction::CMD_WAIT_MANUAL_ANSWER)
				emit commandForManualReaction(cmd,m.args,cfg);
			else if(cfg.act==CommandReaction::CMD_JS_EXEC)
			{
				emit infoMessage("executing js script");
				scriptEng->globalObject().setProperty("args",byteArrayListToJSArray(m.args,scriptEng));
				QScriptValue v=scriptEng->evaluate(cfg.jsScript);
				if(scriptEng->hasUncaughtException())
				{
					QString msg="js exception: "+QString::number(scriptEng->uncaughtExceptionLineNumber())+": ";
					msg+=scriptEng->uncaughtException().toString();
					msg+="\t"+scriptEng->uncaughtExceptionBacktrace().join("\n\t");
					emit infoMessage(msg.toUtf8());
				}
				cfg=CommandReactionConfig();
				if(v.isBool())
				{
					if(v.toBool())
						cfg.act=CommandReaction::CMD_ANSWER_OK;
					else cfg.act=CommandReaction::CMD_ANSWER_ERR;
				}
				else if(v.isObject())
				{
					cfg.act=cmdActFromString(v.property("action").toString());
					QScriptValue arr=v.property("value");
					cfg.retVal.clear();
					quint32 i=0;
					while(1)
					{
						QScriptValue vv=arr.property(i);
						if(!vv.isValid())break;
						cfg.retVal.append(vv.toString().toUtf8());
						++i;
					}
					QScriptValue stateChangeBeforeAnswerVal=v.property("stateChangeBeforeAnswer");
					QScriptValue stateChangeAfterAnswerVal=v.property("stateChangeAfterAnswer");
					applyFromJsToStateMap(stateChangeBeforeAnswerVal,cfg.stateChangeBeforeAnswer);
					applyFromJsToStateMap(stateChangeAfterAnswerVal,cfg.stateChangeAfterAnswer);
				}
			}

			//2-nd step, only direct actions
			if(cfg.act==CommandReaction::CMD_ANSWER_OK||cfg.act==CommandReaction::CMD_ANSWER_ERR)
			{
				auto toCmd=[this](const QByteArray &cmd,quint32 paramIndex,QByteArray &value)
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
			else if(cfg.act==CommandReaction::DEV_STUCK)
			{
				emit warningMessage("device is stuck when processing command");
				setWorking(false);
			}
			else if(cfg.act==CommandReaction::DEV_RESET)
			{
				emit warningMessage("device is reset when processing command");
				QThread::sleep(1);
				resetDevice();
			}
			//else ignoring
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

void TestDevice::onNewSrvReadyMessage(Message m)
{
	if(m.title=="server_ready"&&m.args.count()>=2)
		emit serverFound(bCastSerderAddr,QUuid(m.args[0]),m.args[1]);
}

void TestDevice::setWorking(bool w)
{
	if(working==w)return;
	working=w;
	if(working)
		resetDevice();
	emit workingChanged();
}

bool TestDevice::isWorking()
{
	return working;
}

bool TestDevice::isConnected()
{
	return socket->state()==QAbstractSocket::ConnectedState;
}

void TestDevice::writeMsg(const Message &m)
{
	QByteArray mm=m.dump();
	mm.chop(1);
	emit dbgMessage("send msg: "+mm);
	socket->write(m.dump());
	socket->flush();
}

void TestDevice::setupUidAndName(const QUuid &id,const QByteArray &name)
{
	if(working)return;
	devId=id;
	devName=name;
}

void TestDevice::setSensors(const QByteArray &s)
{
	if(working)return;
	sensorsStr=s;
}

void TestDevice::setControls(const QByteArray &s)
{
	if(working)return;
	controlsStr=s;
}

QUuid TestDevice::id()const
{
	return devId;
}

QByteArray TestDevice::name()const
{
	return devName;
}

QByteArray TestDevice::sensors()
{
	return sensorsStr;
}

QByteArray TestDevice::controls()
{
	return controlsStr;
}

QHostAddress TestDevice::serverAddr()
{
	return socket->peerAddress();
}

void TestDevice::save(const QString &path)
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
		QByteArray rv;
		for(const auto &s:cfg.retVal)
		{
			rv+=Message::escape(s);
			rv+="|";
		}
		if(!rv.isEmpty())
			rv.chop(1);
		QDomElement cmdElem=doc.createElement("command_reaction");
		commandsElem.appendChild(cmdElem);
		cmdElem.setAttribute("command",QString::fromUtf8(i.key()));
		cmdElem.setAttribute("action",cmdActToString(cfg.act));
		cmdElem.setAttribute("return",QString::fromUtf8(rv));
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

void TestDevice::load(const QString &path)
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
		StreamParser::tryParse("|"+cmdElem.attribute("return").toUtf8()+"\n",m);
		cfg.act=cmdActFromString(cmdElem.attribute("action"));
		cfg.retVal=m.args;
		cfg.jsScript=cmdElem.attribute("js_script").toUtf8();
		stateMapFromXml(scbaElem,cfg.stateChangeBeforeAnswer);
		stateMapFromXml(scaaElem,cfg.stateChangeAfterAnswer);
		commands[command]=cfg;
	}

	QDomElement startupStateElem=root.firstChildElement("startup_state");
	stateMapFromXml(startupStateElem,startupState);
}

void TestDevice::sendSensorValue(const QByteArray &sensor,const QByteArrayList &value)
{
	writeMsg(Message("meas",QByteArrayList()<<sensor<<value));
}

void TestDevice::resetDevice()
{
	state.state=startupState;
	delete scriptEng;
	scriptEng=new QScriptEngine;
	scriptEng->evaluate("script=this;");
	timers=new JSTimers(scriptEng,scriptEng);
	scriptEng->globalObject().setProperty("Timer",scriptEng->newQObject(timers),QScriptValue::ReadOnly);
	parser.reset();
	socket->write("0",1);
	socket->flush();
}

void TestDevice::applyToStateMap(DeviceStateMap &map, std::function<void(const QByteArray&,quint32,QByteArray&)> toCmdState,
	std::function<void(const QByteArray&,QByteArray&)> toAdditState)
{
	for(auto i=map.commands.begin();i!=map.commands.end();++i)
	{
		QMap<quint32,QByteArray> &cmdMap=i.value();
		for(auto j=cmdMap.begin();j!=cmdMap.end();++j)
			toCmdState(i.key(),j.key(),j.value());
	}
	for(auto i=map.additionalParams.begin();i!=map.additionalParams.end();++i)
		toAdditState(i.key(),i.value());
}

void TestDevice::applyFromJsToStateMap(QScriptValue &val,DeviceStateMap &map)
{
	if(!val.isObject())return;
	QScriptValue commandsObj=val.property("commands");
	if(commandsObj.isObject())
	{
		QScriptValueIterator cmdIt(commandsObj);
		while(cmdIt.hasNext())
		{
			cmdIt.next();
			QByteArray command=cmdIt.name().toUtf8();
			QMap<quint32,QByteArray> &paramsMap=map.commands[command];
			QScriptValue cmdObj=cmdIt.value();
			QScriptValueIterator paramIt(cmdObj);
			while(paramIt.hasNext())
			{
				paramIt.next();
				bool ok=false;
				quint32 paramIndex=paramIt.name().toUInt(&ok);
				if(!ok)continue;
				paramsMap[paramIndex]=paramIt.value().toString().toUtf8();
			}
		}
	}
	QScriptValue additParamsObj=val.property("additional_params");
	if(additParamsObj.isObject())
	{
		QScriptValueIterator paramIt(additParamsObj);
		while(paramIt.hasNext())
		{
			paramIt.next();
			map.additionalParams[paramIt.name().toUtf8()]=paramIt.value().toString().toUtf8();
		}
	}
}

void TestDevice::writeOk(const QByteArrayList &args)
{
	writeMsg(Message("ok",args));
}

void TestDevice::writeErr(const QByteArrayList &args)
{
	writeMsg(Message("err",args));
}

void TestDevice::onBCastCliReadyRead()
{
	while(bCastCli->hasPendingDatagrams())
	{
		QNetworkDatagram dg=bCastCli->receiveDatagram();
		bCastSerderAddr=dg.senderAddress();
		srvReadyParser.pushData(dg.data());
	}
}

void TestDevice::onSocketReadyRead()
{
	parser.pushData(socket->readAll());
}

void TestDevice::onSyncTimer()
{
	if(disconnectOnSyncTimeout)
	{
		emit warningMessage("sync timeout, disconnecting");
		socket->disconnectFromHost();
	}
	else emit warningMessage("sync timeout, ignoring");
}

void TestDevice::onSocketConnected()
{
	if(working)
		syncTimer->start();
	emit infoMessage("connected to server");
	emit connected();
}

void TestDevice::onSocketDisconnected()
{
	if(working)
		syncTimer->stop();
	emit infoMessage("disconnected from server");
	emit disconnected();
}
