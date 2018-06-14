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
	currentFilledStr=&newMessage.title;
	hexChars.resize(2);
	state=NORMAL;
}

void ARpcStreamParser::pushData(const QByteArray &data)
{
	for(int i=0;i<data.size();++i)
	{
		if(data[i]==0)
		{
			reset();
			continue;
		}
		if(state==ESCAPE)
			parseCharInEscapeState(data[i]);
		else if(state==ESCAPE_HEX1)
		{
			hexChars[0]=data[i];
			state=ESCAPE_HEX2;
		}
		else if(state==ESCAPE_HEX2)
		{
			hexChars[1]=data[i];
			state=NORMAL;
			bool ok=false;
			short cc=hexChars.toShort(&ok,16);
			if(ok)currentFilledStr->append((char)cc);
		}
		else //NORMAL
			parseCharInNormalState(data[i]);
	}
}

void ARpcStreamParser::reset()
{
	currentFilledStr=&newMessage.title;
	newMessage.args.clear();
	newMessage.title.clear();
	state=NORMAL;
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

QByteArray ARpcStreamParser::dump(const ARpcMessage &m)
{
	QByteArray retVal;
	retVal.append(ARpcMessage::escape(m.title));
	for(const QByteArray &a:m.args)
	{
		retVal.append(ARpcConfig::argDelim);
		retVal.append(ARpcMessage::escape(a));
	}
	retVal.append(ARpcConfig::msgDelim);
	return retVal;
}

void ARpcStreamParser::parseCharInNormalState(char c)
{
	if(c=='\\')
		state=ESCAPE;
	else if(c=='|')
	{
		newMessage.args.append(QByteArray());
		currentFilledStr=&newMessage.args.last();
	}
	else if(c=='\n')
	{
		ARpcMessage m=newMessage;
		newMessage.title.clear();
		newMessage.args.clear();
		currentFilledStr=&newMessage.title;
		if(!m.title.isEmpty())
		{
			if(fHandler)
				fHandler(m);
			if(cHandler)
				cHandler->processMessage(m);
			emit processMessage(m);
		}
	}
	else
		currentFilledStr->append(c);
}

void ARpcStreamParser::parseCharInEscapeState(char c)
{
	if(c=='\\')
		currentFilledStr->append('\\');
	else if(c=='n')
		currentFilledStr->append('\n');
	else if(c=='|')
		currentFilledStr->append('|');
	else if(c=='0')
		currentFilledStr->append(char(0));
	else if(c=='x')
	{
		state=ESCAPE_HEX1;
		return;
	}
	else
		currentFilledStr->append(c);
	state=NORMAL;
}
