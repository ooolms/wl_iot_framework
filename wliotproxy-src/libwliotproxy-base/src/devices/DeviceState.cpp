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

QByteArrayList DeviceState::dumpToMsgArgs()
{
	QByteArrayList retVal;
	for(auto i=commandParams.begin();i!=commandParams.end();++i)
	{
		QByteArray command=i.key();
		const QMap<int,QByteArray> &params=i.value();
		for(auto j=params.begin();j!=params.end();++j)
		{
			retVal.append(command);
			retVal.append(QByteArray::number(j.key()));
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
