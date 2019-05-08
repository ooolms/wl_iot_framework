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

#include "wliot/devices/StreamParser.h"
#include <QDebug>

StreamParser::StreamParser(QObject *parent)
	:QObject(parent)
{
	currentFilledStr=&nextMessage.title;
	hexChars.resize(2);
	state=NORMAL;
}

void StreamParser::pushData(const QByteArray &data)
{
	for(int i=0;i<data.size();++i)
	{
		if(data[i]==0)
		{
			reset();
			emit streamWasReset();
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

void StreamParser::reset()
{
	currentFilledStr=&nextMessage.title;
	nextMessage.args.clear();
	nextMessage.title.clear();
	state=NORMAL;
}

void StreamParser::parseCharInNormalState(char c)
{
	if(c=='\\')
		state=ESCAPE;
	else if(c=='|')
	{
		nextMessage.args.append(QByteArray());
		currentFilledStr=&nextMessage.args.last();
	}
	else if(c=='\n')
	{
		Message m=nextMessage;
		nextMessage.title.clear();
		nextMessage.args.clear();
		currentFilledStr=&nextMessage.title;
		if(!m.title.isEmpty())
			emit newMessage(m);
	}
	else
		currentFilledStr->append(c);
}

void StreamParser::parseCharInEscapeState(char c)
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
