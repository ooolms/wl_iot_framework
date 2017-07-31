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
