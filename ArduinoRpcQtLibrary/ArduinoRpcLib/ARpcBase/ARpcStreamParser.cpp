#include "ARpcStreamParser.h"
#include <QDebug>

ARpcStreamParser::ARpcStreamParser(QObject *parent)
	:QObject(parent)
{
	msgParser=new ARpcMessageParser(this);
}

void ARpcStreamParser::pushData(const QString &data)
{
	buffer.append(data);
	int index=buffer.indexOf(ARpcConfig::msgDelim);
	while(index!=-1)
	{
		QString msgText=buffer.mid(0,index);
		if(msgText.endsWith('\r'))msgText.chop(1);
		ARpcMessage m=msgParser->parse(msgText);
		if(!m.title.isEmpty())emit processMessage(m);
		buffer.remove(0,index+1);
		index=buffer.indexOf(ARpcConfig::msgDelim);
	}
}

void ARpcStreamParser::reset()
{
	buffer.clear();
}
