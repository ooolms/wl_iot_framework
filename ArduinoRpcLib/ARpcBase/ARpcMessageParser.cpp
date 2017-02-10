#include "ARpcMessageParser.h"
#include <QDebug>

ARpcMessageParser::ARpcMessageParser(QObject *parent)
	:QObject(parent)
{
}

ARpcMessage ARpcMessageParser::parse(const QString &str)const
{
	ARpcMessage m;
	int index=0,newIndex=0;
	newIndex=str.indexOf(ARpcConfig::argDelim,index);
	if(newIndex==-1)
	{
		m.title=str;
		return m;
	}
	m.title=str.mid(0,newIndex);
	index=newIndex;
	while(newIndex!=-1)
	{
		newIndex=str.indexOf(ARpcConfig::argDelim,index+1);
		m.args.append(str.mid(index+1,newIndex-index-1));
		index=newIndex;
	}
	qDebug()<<"MSG: "<<m.title<<"; ARGS: "<<m.args;
	return m;
}

QString ARpcMessageParser::dump(const ARpcMessage &m)const
{
	if(m.args.isEmpty())return m.title;
	return m.title+ARpcConfig::argDelim+m.args.join(ARpcConfig::argDelim);
}
