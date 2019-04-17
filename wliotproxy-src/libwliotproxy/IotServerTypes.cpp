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

#include "IotServerTypes.h"

QString IotServerDevicePolicyNote::polToStr()const
{
	QString s;
	if(pol&IotServerDevicePolicyFlag::READ_STORAGES)
		s+="r";
	if(pol&IotServerDevicePolicyFlag::SETUP_STORAGES)
		s+="m";
	if(pol&IotServerDevicePolicyFlag::READ_STATE)
		s+="s";
	if(pol&IotServerDevicePolicyFlag::EXECUTE_COMMANDS)
		s+="e";
	return s;
}
