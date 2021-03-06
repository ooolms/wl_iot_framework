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

#ifndef APRCSTREAMWRITER_H
#define APRCSTREAMWRITER_H

#include "ARpcIWriteCallback.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

class ARpcStreamWriter
{
public:
	explicit ARpcStreamWriter(ARpcIWriteCallback *wcb);
	virtual ~ARpcStreamWriter(){}
	void resetStream();

	bool writeMsg(const char *msg,const char **args,unsigned char argsCount);
	bool writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);

	bool beginWriteMsg();
	void writeArg(const char *arg,unsigned long sz);
	void writeArg(const char *arg);
	void writeArgNoEscape(const char *arg);
#ifdef ARDUINO
	void writeArgNoEscape(const __FlashStringHelper *arg);
#endif
	void writeArgNoEscape(const char *arg,unsigned long sz);
	void endWriteMsg();

protected:
	virtual void writeMsgHeaders();

private:
	void writeDataEscaped(const char *data,unsigned long sz);

protected:
	ARpcIWriteCallback *writeCallback;
	bool needArgDelim;
	bool msgFinished;
};

#endif
