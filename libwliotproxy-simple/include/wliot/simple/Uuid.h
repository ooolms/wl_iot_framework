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

#ifndef UUID_H
#define UUID_H

class Uuid
{
public:
	Uuid();
	Uuid(const Uuid &t);
	Uuid(const char *str);
	void parse(const char *str);
	bool isValid()const;
	void toString(char str[39])const;
	void toHex(char str[33])const;
	void getBinary(unsigned char id[16])const;
	void fromBinary(const unsigned char id[16]);
	bool operator==(const Uuid &id)const;
	bool operator!=(const Uuid &t)const;
	Uuid& operator=(const Uuid &t);

public:
	static inline unsigned char byteFromHexChar(unsigned char c)
	{
		if(c>='0'&&c<='9')
			return c-'0';
		else if(c>='a'&&c<='f')
			return c-87;//-'a'+10
		else if(c>='A'&&c<='F')
			return c-55;//-'A'+10
		else return 0xff;
	}

	static inline void byteTo2HexChars(unsigned char c,unsigned char &c1,unsigned char &c2)
	{
		c2=c;
		c=c&0xf;
		if(c<=9)
			c1=c+'0';
		else c1=c-10+'a';
		c=c2>>4;
		if(c<=9)
			c2=c+'0';
		else c2=c-10+'a';
	}

private:
	void parseRfc(const char *str);
	void parseHex(const char *str);
	bool parseHexPart(const char *str,int strOffset,int uuidOffset,int bytesCount);
	void writeHexPart(char *str,int strOffset,int uuidOffset,int bytesCount)const;

public:
	static const int idStringLen=39;
	static const int idHexLen=33;
	static const int idBinLen=16;

private:
	bool valid;
	unsigned char uuid[16];
};

#endif // UUID_H
