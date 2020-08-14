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

#include "wliot/client/VirtualDeviceCallback.h"
#include "wliot/client/VirtualDeviceClient.h"

using namespace WLIOT;
using namespace WLIOTClient;

VirtualDeviceCallback::VirtualDeviceCallback()
{
	mDev=0;
}

void VirtualDeviceCallback::setDevClient(VirtualDeviceClient *cli)
{
	mDev=cli;
}

void VirtualDeviceCallback::prepareState(DeviceState &)
{
}

void VirtualDeviceCallback::commandParamStateChanged(
	const QByteArray &cmd,quint32 paramIndex,const QByteArray &value)
{
	if(mDev)mDev->commandParamStateChanged(cmd,paramIndex,value);
}

void VirtualDeviceCallback::additionalStateChanged(
	const QByteArray &paramName,const QByteArray &value)
{
	if(mDev)mDev->additionalStateChanged(paramName,value);
}
