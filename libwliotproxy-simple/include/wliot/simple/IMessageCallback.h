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

#ifndef IMESSAGECALLBACK_H
#define IMESSAGECALLBACK_H

class IMessageCallback
{
public:
	virtual ~IMessageCallback(){}
	virtual void processMsg(const char *msg,const char **args,unsigned char argsCount)=0;
};

#endif // IMESSAGECALLBACK_H
