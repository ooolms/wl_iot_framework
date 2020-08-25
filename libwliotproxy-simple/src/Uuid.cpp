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

#include "wliot/simple/Uuid.h"
#include <stdio.h>
#include <string.h>

Uuid::Uuid()
{
	memset(uuid,0,16);
	valid=false;
}

Uuid::Uuid(const Uuid &t)
{
	valid=t.valid;
	memcpy(uuid,t.uuid,16);
}

Uuid::Uuid(const char *str)
{
	memset(uuid,0,16);
	parse(str);
}

void Uuid::parse(const char *str)
{
	valid=false;
	if(strlen(str)<32)return;
	if(str[0]=='{')
		parseRfc(str);
	else parseHex(str);
}

bool Uuid::isValid()const
{
	return valid;
}

void Uuid::toString(char str[])const
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

void Uuid::toHex(char str[])const
{
	str[32]=0;
	writeHexPart(str,0,0,16);
}

void Uuid::getBinary(unsigned char id[])const
{
	memcpy(id,uuid,16);
}

void Uuid::fromBinary(const unsigned char id[])
{
	memcpy(uuid,id,16);
	valid=true;
}

bool Uuid::operator==(const Uuid &id)const
{
	if(!valid||!id.valid)return false;
	return memcmp(uuid,id.uuid,16)==0;
}

bool Uuid::operator!=(const Uuid &t)const
{
	return !(*this==t);
}

Uuid &Uuid::operator=(const Uuid &t)
{
	valid=t.valid;
	memcpy(uuid,t.uuid,16);
	return *this;
}

void Uuid::parseRfc(const char *str)
{
	if(strlen(str)!=38)return;
	if(str[0]!='{'||str[9]!='-'||str[14]!='-'||str[19]!='-'||str[24]!='-'||str[37]!='}')
		return;
	if(parseHexPart(str,1,0,4)&&parseHexPart(str,10,4,2)&&parseHexPart(str,15,6,2)&&
		parseHexPart(str,20,8,2)&&parseHexPart(str,25,10,6))
		valid=true;
}

void Uuid::parseHex(const char *str)
{
	if(strlen(str)!=32)return;
	if(parseHexPart(str,0,0,16))
		valid=true;
}

bool Uuid::parseHexPart(const char *str,int strOffset,int uuidOffset,int bytesCount)
{
	for(int i=0;i<bytesCount;++i)
	{
		unsigned char c1,c2;
		c1=byteFromHexChar(str[2*i+1+strOffset]);
		c2=byteFromHexChar(str[2*i+strOffset]);
		if(c1==0xff||c2==0xff)
		{
			memset(uuid,0,16);
			return false;
		}
		uuid[i+uuidOffset]=c1+(c2<<4);
	}
	return true;
}

void Uuid::writeHexPart(char *str,int strOffset,int uuidOffset,int bytesCount)const
{
	for(int i=0;i<bytesCount;++i)
	{
		unsigned char c1,c2;
		byteTo2HexChars(uuid[i+uuidOffset],c1,c2);
		str[2*i+1+strOffset]=(char)c1;
		str[2*i+strOffset]=(char)c2;
	}
}
