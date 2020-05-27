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

#include "IotkitAgentCommandSource.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static const quint16 agentCommandPort=41235;//UDP port, agent write commands here

IotkitAgentCommandSource::IotkitAgentCommandSource(QObject *parent)
	:IExternCommandSource(parent)
{
	commandReadSock.bind(QHostAddress::LocalHost,agentCommandPort);
	QObject::connect(&commandReadSock,SIGNAL(readyRead()),
		this,SLOT(onDataFromCommandSocket()));
}

void IotkitAgentCommandSource::onDataFromCommandSocket()
{
	while(commandReadSock.hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(commandReadSock.pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;
		commandReadSock.readDatagram(datagram.data(),datagram.size(),&sender,&senderPort);
		parseCommand(datagram);
	}
}

void IotkitAgentCommandSource::parseCommand(const QByteArray &data)
{
	struct ArgForSorting
	{
		ArgForSorting(int k,const QString &v)
		{
			key=k;
			value=v;
		}
		ArgForSorting(){key=0;}
		bool operator<(const ArgForSorting &a)const
		{
			return key<a.key;
		}

	public:
		int key;
		QString value;
	};

	QJsonDocument doc=QJsonDocument::fromJson(data);
	if(doc.isNull())return;
	if(!doc.isObject())return;
	QJsonObject obj=doc.object();
	QJsonValue commandVal=obj["command"];
	QJsonValue componentVal=obj["component"];
	QJsonValue argvVal=obj["argv"];
	if(!commandVal.isString()||!componentVal.isString()||!argvVal.isArray())return;
	QByteArray command=commandVal.toString().toUtf8();
	QByteArray component=componentVal.toString().toUtf8();
	QMap<QString,QString> args;
	QJsonArray argvArr=argvVal.toArray();
	for(int i=0;i<argvArr.count();++i)
	{
		QJsonObject obj=argvArr[i].toObject();
		if(obj.isEmpty())continue;
		QJsonValue nameVal=obj["name"];
		QJsonValue valueVal=obj["value"];
		if(!nameVal.isString()||!valueVal.isString())continue;
		args[nameVal.toString()]=valueVal.toString();
	}
	if(component.isEmpty()||command.isEmpty())return;
	QList<ArgForSorting> argsTmp;
	for(auto i=args.begin();i!=args.end();++i)
	{
		bool ok=false;
		int k=i.key().toInt(&ok);
		if(ok)argsTmp.append(ArgForSorting(k,i.value()));
	}
	qSort(argsTmp);
	QByteArrayList argsForDev;
	for(auto &a:argsTmp)
		argsForDev.append(a.value.toUtf8());
	QByteArrayList retVal;
	execCommand(component,command,argsForDev,retVal);
}

