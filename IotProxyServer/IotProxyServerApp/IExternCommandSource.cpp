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

#include "IExternCommandSource.h"
#include "ARpcBase/ARpcUnsafeCall.h"
#include "IotProxyInstance.h"
#include <QDebug>

IExternCommandSource::IExternCommandSource(QObject *parent)
	:QObject(parent)
{
}

bool IExternCommandSource::execCommand(const QString &devIdOrName,const QString &command,
	const QStringList &arguments,QStringList &retVal)
{
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev)return false;
	qDebug()<<"Exec command on device: "<<dev->id().toString()<<"; "<<command<<"|"<<arguments.join("|");
	ARpcUnsafeCall call;
	return call.call(dev,command,arguments,retVal);
}

