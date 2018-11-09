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

#ifndef ARPCIDEVEVENTSCALLBACK_H
#define ARPCIDEVEVENTSCALLBACK_H

#include "ARpcUuid.h"

class ARpcIDevEventsCallback
{
public:
	virtual ~ARpcIDevEventsCallback(){}
	virtual void processCommand(const char *cmd,const char **args,unsigned char argsCount)=0;
	virtual void onSyncMsg(){}
	virtual void onFirstSetupCmd(const ARpcUuid &uuid,const char *name){(void)uuid;(void)name;}
};

#endif // ARPCIDEVEVENTSCALLBACK_H
