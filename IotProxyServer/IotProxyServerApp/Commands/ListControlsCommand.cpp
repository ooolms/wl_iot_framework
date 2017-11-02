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

#include "ListControlsCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

ListControlsCommand::ListControlsCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool ListControlsCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title=="list_controls")
	{
		if(m.args.count()<1)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(m.args[0]);
		if(dev==0)
		{
			retVal.append(StandardErrors::noDeviceWithId.arg(m.args[0]));
			return false;
		}
		ARpcControlsGroup controls;
		if(!dev->getControlsDescription(controls))
		{
			retVal.append("can't get controls from device");
			return false;
		}
		QString xmlData;
		ARpcControlsGroup::dumpToXml(xmlData,controls);
		retVal.append(xmlData);
		return true;
	}
	return false;
}

QStringList ListControlsCommand::acceptedCommands()
{
	return QStringList()<<"list_controls";
}

