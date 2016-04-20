#include "ARpcStreamParser.h"
#include <QDebug>

ARpcStreamParser::ARpcStreamParser(const ARpcConfig &cfg,ARpcMessageParser *mParser,QObject *parent)
	:QObject(parent)
{
	config=cfg;
	msgParser=mParser;
}

void ARpcStreamParser::pushData(const QString &data)
{
	qDebug()<<"RECEIVED: "<<data;
	buffer.append(data);
	int index=buffer.indexOf(config.msgDelim);
	while(index!=-1)
	{
		QString msgText=buffer.mid(0,index);
		ARpcMessage m=msgParser->parse(msgText);
		if(!m.title.isEmpty())emit processMessage(m);
		buffer.remove(0,index+1);
		index=buffer.indexOf(config.msgDelim);
	}
}

void ARpcStreamParser::reset()
{
	buffer.clear();
}
