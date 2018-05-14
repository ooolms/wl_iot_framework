/*******************************************
Copyright 2018 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ARPCSTREAMPARSER_H
#define ARPCSTREAMPARSER_H

#include "ARpcIMessageCallback.h"

class ARpcStreamParser
{
private:
	ARpcStreamParser(const ARpcStreamParser &);

public:
	ARpcStreamParser(unsigned long bSize,ARpcIMessageCallback *mcb);
	ARpcStreamParser(char *buf,unsigned long bSize,ARpcIMessageCallback *mcb);
	~ARpcStreamParser();
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void reset();

private:
	inline void parseCharInNormalState(char c);
	inline void parseCharInEscapeState(char c);

private:
	ARpcIMessageCallback *messageCallback;

private:
#ifdef ARPC_MAX_ARGS_COUNT
	static const unsigned char maxArgsCount=ARPC_MAX_ARGS_COUNT;//максимальное число аргументов
#else
	static const unsigned char maxArgsCount=20;//максимальное число аргументов
#endif
	char *buffer;//буфер
	bool extBuf;
	char *args[maxArgsCount];
	unsigned char hexChars[2];
	unsigned long bufSize;//размер буфера
	unsigned long bufIndex;
	unsigned char currentArgIndex;//1-based index, 0 - msg title
	enum
	{
		NORMAL,
		ESCAPE,//next symbol is escaped
		ESCAPE_HEX1,//next symbol is first of 2-letter hex code
		ESCAPE_HEX2//next symbol is second of 2-letter hex code
	}state;
};

#endif // ARPCSTREAMPARSER_H
