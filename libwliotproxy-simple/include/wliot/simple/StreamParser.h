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

#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include "wliot/simple/IMessageCallback.h"

class StreamParser
{
private:
	StreamParser(const StreamParser &);

public:
	StreamParser(unsigned long bSize,IMessageCallback *mcb);
	~StreamParser();
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void reset();

private:
	inline void parseCharInNormalState(char c);
	inline void parseCharInEscapeState(char c);

private:
	IMessageCallback *messageCallback;

private:
	char *buffer;//буфер
	char *args[30];
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

#endif // STREAMPARSER_H
