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

#include "SerialNotificator.h"

using namespace WLIOT;

SerialNotificator::SerialNotificator()
{
	w.addPath("/dev");
	connect(&w,&QFileSystemWatcher::directoryChanged,this,&SerialNotificator::checkSerialPorts);
}

SerialNotificator::~SerialNotificator()
{
	w.removePath("dev");
}

SerialNotificator& SerialNotificator::inst()
{
	static SerialNotificator notificator;
	return notificator;
}
