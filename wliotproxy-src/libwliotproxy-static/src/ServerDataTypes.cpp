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

#include "wliot/client/ServerDataTypes.h"

using namespace WLIOT;
using namespace WLIOTClient;

QString ApmDevicePolicyNote::polToStr()const
{
	QString s;
	if(pol&ApmDevicePolicyFlag::READ_STORAGES)
		s+="r";
	if(pol&ApmDevicePolicyFlag::SETUP_STORAGES)
		s+="m";
	if(pol&ApmDevicePolicyFlag::READ_STATE)
		s+="s";
	if(pol&ApmDevicePolicyFlag::EXECUTE_COMMANDS)
		s+="e";
	return s;
}
