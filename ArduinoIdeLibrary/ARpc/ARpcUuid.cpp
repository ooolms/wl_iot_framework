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

#include "ARpcUuid.h"
#include <stdio.h>
#include <string.h>

const int ARpcUuid::uuidLen=16;
const int ARpcUuid::uuidRfcStringLen=38;
const int ARpcUuid::uuidHexStringLen=32;

ARpcUuid::ARpcUuid()
{
	memset(uuid,0,16);
}

ARpcUuid::ARpcUuid(const ARpcUuid &t)
{
	memcpy(uuid,t.uuid,16);
}

ARpcUuid::ARpcUuid(const char *str)
{
	memset(uuid,0,16);
	parse(str);
}

uint8_t* ARpcUuid::dataPtr()
{
	return uuid;
}

void ARpcUuid::parse(const char *str)
{
	if(strlen(str)<32)return;
	if(str[0]=='{')
		parseRfc(str);
	else parseHex(str);
}

bool ARpcUuid::isValid()const
{
	for(int i=0;i<16;++i)
		if(uuid[i]!=0)return true;
	return false;
}

void ARpcUuid::toString(char str[])const
{
	str[0]='{';
	str[37]='}';
	str[9]=str[14]=str[19]=str[24]='-';
	str[38]=0;
	writeHexPart(str,1,0,4);
	writeHexPart(str,10,4,2);
	writeHexPart(str,15,6,2);
	writeHexPart(str,20,8,2);
	writeHexPart(str,25,10,6);
}

void ARpcUuid::toHex(char str[])const
{
	str[32]=0;
	writeHexPart(str,0,0,16);
}

bool ARpcUuid::operator==(const ARpcUuid &id)const
{
	return memcmp(uuid,id.uuid,16)==0;
}

bool ARpcUuid::operator!=(const ARpcUuid &t)const
{
	return !(*this==t);
}

ARpcUuid &ARpcUuid::operator=(const ARpcUuid &t)
{
	memcpy(uuid,t.uuid,16);
	return *this;
}

void ARpcUuid::parseRfc(const char *str)
{
	if(strlen(str)!=38)return;
	if(str[0]!='{'||str[9]!='-'||str[14]!='-'||str[19]!='-'||str[24]!='-'||str[37]!='}')
		return;
	if(!parseHexPart(str,1,0,4)||!parseHexPart(str,10,4,2)||!parseHexPart(str,15,6,2)||
		!parseHexPart(str,20,8,2)||!parseHexPart(str,25,10,6))
		memset(uuid,0,16);
}

void ARpcUuid::parseHex(const char *str)
{
	if(strlen(str)!=32)return;
	if(!parseHexPart(str,0,0,16))
		memset(uuid,0,16);
}

bool ARpcUuid::parseHexPart(const char *str,int strOffset,int uuidOffset,int bytesCount)
{
	for(int i=0;i<bytesCount;++i)
	{
		uint8_t c1,c2;
		c1=byteFromHexChar(str[2*i+1+strOffset]);
		c2=byteFromHexChar(str[2*i+strOffset]);
		if(c1==0xff||c2==0xff)
		{
			memset(uuid,0,16);
			return false;
		}
		uuid[i+uuidOffset]=c1+(uint8_t)(c2<<4);
	}
	return true;
}

void ARpcUuid::writeHexPart(char *str,int strOffset,int uuidOffset,int bytesCount)const
{
	for(int i=0;i<bytesCount;++i)
	{
		unsigned char c1,c2;
		byteTo2HexChars(uuid[i+uuidOffset],c1,c2);
		str[2*i+1+strOffset]=(char)c1;
		str[2*i+strOffset]=(char)c2;
	}
}
