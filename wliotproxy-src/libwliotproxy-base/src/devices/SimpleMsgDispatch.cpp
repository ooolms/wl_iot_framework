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

#include "wliot/devices/SimpleMsgDispatch.h"
#include "wliot/WLIOTProtocolDefs.h"

SimpleMsgDispatch::SimpleMsgDispatch(QObject *parent)
	:QObject(parent)
{
}

void SimpleMsgDispatch::onNewMessageFromDevice(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::infoMsg&&m.args.count()>=1)
		emit infoMsg(m.args[0]);
	else if(m.title==WLIOTProtocolDefs::measurementMsg&&m.args.count()>=2)
		emit measurementMsg(m.args[0],m.args[1]);
	else if(m.title==WLIOTProtocolDefs::stateChangedMsg&&m.args.count()%3==0)
	{
		for(int i=0;i<m.args.count()/3;++i)
		{
			QByteArray command=m.args[3*i];
			QByteArray nameOrIndex=m.args[3*i+1];
			QByteArray value=m.args[3*i+2];
			if(command.isEmpty())return;
			else if(command=="#")
			{
				if(nameOrIndex.isEmpty())return;
				emit additionalStateChanged(nameOrIndex,value);
			}
			else
			{
				bool ok=false;
				int index=nameOrIndex.toInt(&ok);
				if(!ok||index<=0)return;
				emit commandStateChanged(command,index,value);
			}
		}
	}
}
