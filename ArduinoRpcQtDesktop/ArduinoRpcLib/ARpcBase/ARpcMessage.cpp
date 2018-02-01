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

#include "ARpcMessage.h"

static const int arpcMessageMetaType=qRegisterMetaType<ARpcMessage>("ARpcMessage");

ARpcMessage::ARpcMessage()
{
}

ARpcMessage::ARpcMessage(const QByteArray &t)
{
	title=t;
}

ARpcMessage::ARpcMessage(const QByteArray &t,const QByteArrayList &a)
{
	title=t;
	args=a;
}

QByteArray ARpcMessage::escape(const QByteArray &data)
{
	QByteArray retVal;
	for(int i=0;i<data.count();++i)
	{
		if(data[i]=='\\')
			retVal.append("\\\\");
		else if(data[i]=='|')
			retVal.append("\\|");
		else if(data[i]=='\n')
			retVal.append("\\n");
		else if(data[i]==0)
			retVal.append("\\0");
		else retVal.append(data[i]);
	}
	return retVal;
}
