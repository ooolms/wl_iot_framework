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

#ifndef IENGINECALLBACKS_H
#define IENGINECALLBACKS_H

#include <QUuid>
#include <QByteArrayList>

namespace WLIOTVDIL
{
	class IEngineCallbacks
	{
	public:
		virtual ~IEngineCallbacks(){}
		virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)=0;
		virtual bool commandCallbackWaitAnswer(const QUuid &devId,const QByteArray &cmd,
			const QByteArrayList &args,QByteArrayList &retVal)=0;
		virtual void debugCallback(const QString &msg)=0;
		virtual void sendVDevMeasurementB(const QByteArray &sensorName,const QByteArray &data)=0;
		virtual void vdevAdditionalStateChanged(const QByteArray &paramName,const QByteArray &value)=0;
	};
}

#endif // IENGINECALLBACKS_H
