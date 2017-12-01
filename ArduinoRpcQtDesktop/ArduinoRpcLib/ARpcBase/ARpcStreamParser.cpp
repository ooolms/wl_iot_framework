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

#include "ARpcStreamParser.h"
#include <QDebug>

ARpcStreamParser::ARpcStreamParser(QObject *parent)
	:QObject(parent)
{
	fHandler=0;
	cHandler=0;
}

void ARpcStreamParser::pushData(const QString &data)
{
	buffer.append(data);
	int index=buffer.indexOf(ARpcConfig::msgDelim);
	while(index!=-1)
	{
		QString msgText=buffer.mid(0,index);
		buffer.remove(0,index+1);
		index=buffer.indexOf(ARpcConfig::msgDelim);
//		if(msgText.endsWith('\r'))msgText.chop(1);//HACK !!!
		ARpcMessage m=parseMessage(msgText);
		if(!m.title.isEmpty())
		{
			if(fHandler)
				fHandler(m);
			if(cHandler)
				cHandler->processMessage(m);
			emit processMessage(m);
		}
	}
}

void ARpcStreamParser::reset()
{
	buffer.clear();
}

ARpcIMessageHandler* ARpcStreamParser::setMessageCHandler(ARpcIMessageHandler *h)
{
	std::swap(h,cHandler);
	return h;
}

ARpcStreamParser::MessageHandler ARpcStreamParser::setMessageFHandler(MessageHandler h)
{
	std::swap(h,fHandler);
	return h;
}

QString ARpcStreamParser::dump(const ARpcMessage &m)
{
	if(m.args.isEmpty())return m.title+ARpcConfig::msgDelim;
	return m.title+ARpcConfig::argDelim+m.args.join(ARpcConfig::argDelim)+ARpcConfig::msgDelim;
}

ARpcMessage ARpcStreamParser::parseMessage(const QString &str) const
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
	return m;
}
