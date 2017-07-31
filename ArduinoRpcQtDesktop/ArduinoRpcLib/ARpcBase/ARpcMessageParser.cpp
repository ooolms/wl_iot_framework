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
	return m;
}

QString ARpcMessageParser::dump(const ARpcMessage &m)const
{
	if(m.args.isEmpty())return m.title;
	return m.title+ARpcConfig::argDelim+m.args.join(ARpcConfig::argDelim);
}
