#include "ARpcMessageParser.h"
#include <QDebug>

ARpcMessageParser::ARpcMessageParser(const ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
	,config(cfg)
{
}

ARpcMessage ARpcMessageParser::parse(const QString &str)const
{
	ARpcMessage m;
	int index=0,newIndex=0;
	newIndex=str.indexOf(config.argDelim,index);
	if(newIndex==-1)
	{
		m.title=str;
		return m;
	}
	m.title=str.mid(0,newIndex);
	index=newIndex;
	while(newIndex!=-1)
	{
		newIndex=str.indexOf(config.argDelim,index+1);
		m.args.append(str.mid(index+1,newIndex-index-1));
		index=newIndex;
	}
	qDebug()<<"MSG: "<<m.title<<"; ARGS: "<<m.args;
	return m;
}

QString ARpcMessageParser::dump(const ARpcMessage &m)const
{
	if(m.args.isEmpty())return m.title;
	return m.title+config.argDelim+m.args.join(config.argDelim);
}
