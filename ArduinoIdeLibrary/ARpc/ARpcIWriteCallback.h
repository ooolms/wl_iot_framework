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

#ifndef ARPCIWRITECALLBACK_H
#define ARPCIWRITECALLBACK_H

class ARpcIWriteCallback
{
public:
	virtual ~ARpcIWriteCallback(){}
	virtual void writeData(const char *data,unsigned long sz)=0;
	virtual void writeStr(const char *str)=0;
};

#endif // ARPCIWRITECALLBACK_H
