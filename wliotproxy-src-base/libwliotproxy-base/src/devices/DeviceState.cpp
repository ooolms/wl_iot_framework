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

#include "wliot/devices/DeviceState.h"

using namespace WLIOT;

QByteArrayList DeviceState::dumpToMsgArgs()const
{
	QByteArrayList retVal;
	for(auto i=commandParams.begin();i!=commandParams.end();++i)
	{
		QByteArray command=i.key();
		const auto &params=i.value();
		for(auto j=params.begin();j!=params.end();++j)
		{
			retVal.append(command);
			retVal.append(QByteArray::number(j.key(),'g',200));
			retVal.append(j.value());
		}
	}
	for(auto i=additionalAttributes.begin();i!=additionalAttributes.end();++i)
	{
		retVal.append("#");
		retVal.append(i.key());
		retVal.append(i.value());
	}
	return retVal;
}

bool DeviceState::parseMsgArgs(const QByteArrayList &args)
{
	if(args.count()%3!=0)return false;
	QMap<QByteArray,QMap<quint32,QByteArray>> newCmdAttrs;
	QMap<QByteArray,QByteArray> newAdditAttrs;
	for(int i=0;i<args.count()/3;++i)
	{
		QByteArray command=args[3*i];
		QByteArray nameOrIndex=args[3*i+1];
		QByteArray value=args[3*i+2];
		if(command.isEmpty())return false;
		else if(command=="#")
		{
			if(nameOrIndex.isEmpty())return false;
			newAdditAttrs[nameOrIndex]=value;
		}
		else
		{
			bool ok=false;
			quint32 index=nameOrIndex.toUInt(&ok);
			if(!ok)return false;
			newCmdAttrs[command][index]=value;
		}
	}
	commandParams=newCmdAttrs;
	additionalAttributes=newAdditAttrs;
	return true;
}

DeviceState DeviceState::makeFromCommands(const QList<ControlsCommand> &commands)
{
	DeviceState st;
	for(const ControlsCommand &c:commands)
	{
		for(quint32 j=0;j<(quint32)c.params.count();++j)
		{
			const ControlsCommandParam &p=c.params[(int)j];
			auto &map=st.commandParams[c.commandToExec];
			if(p.type==ControlsCommandParam::CHECKBOX)
				map[j]=p.attributes.value("offValue","0");
			else if(p.type==ControlsCommandParam::TEXT_EDIT)
				map[j]=p.attributes.value("placeholder","0");
			else if(p.type==ControlsCommandParam::SELECT)
				map[j]=p.attributes.value("values","0").split('|').value(0,"0");
			else if(p.type==ControlsCommandParam::SLIDER)
				map[j]=p.attributes.value("min","0");
			else if(p.type==ControlsCommandParam::DIAL)
				map[j]=p.attributes.value("min","0");
			else if(p.type==ControlsCommandParam::RADIO)
				map[j]=p.attributes.value("values","0").split('|').value(0,"0");
			else if(p.type==ControlsCommandParam::HIDDEN)
				map[j]=p.attributes.value("value","0");
			else map[j]="0";
		}
	}
	return st;
}
