//from here http://base64.sourceforge.net/b64.c

/*
** Translation Table (RFC1113)
*/
static const char base64_enc_tbl[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//static const char base64_dec_tbl[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
//static const char base64_dec_tbl[]="+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxy";
static const char base64_dec_tbl[]={
	0x3e,0xff,0xff,0xff,0x3f,0x34,0x35,0x36,0x37,0x38,
	0x39,0x3a,0x3b,0x3c,0x3d,0xff,0xff,0xff,0x00,0xff,
	0xff,0xff,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,
	0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xff,0xff,
	0xff,0xff,0xff,0xff,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
	0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,
};

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
static void base64_encodeblock(const unsigned char *in,unsigned char *out,int len)
{
	out[0]=out[1]=out[2]=out[3]='=';
	if(len>0)
	{
		out[0]=(unsigned char)base64_enc_tbl[(int)(in[0]>>2)];
		if(len>1)
		{
			out[1]=(unsigned char)base64_enc_tbl[(int)(((in[0]&0x03)<<4)|((in[1]&0xf0)>>4))];
			if(len>2)
			{
				out[2]=(unsigned char)base64_enc_tbl[(int)(((in[1]&0x0f)<<2)|((in[2]&0xc0)>>6))];
				out[3]=(unsigned char)base64_enc_tbl[(int)(in[2]&0x3f)];
			}
			else out[2]=base64_enc_tbl[(int)((in[1]&0x0f)<<2)];
		}
		else out[1]=(unsigned char)base64_enc_tbl[(int)((in[0]&0x03)<<4)];
	}
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
** len - length of "out"
*/
static void base64_decodeblock(const unsigned char *in,unsigned char *out,int len)
{
	out[0]=(unsigned char)(base64_dec_tbl[in[0]-43]<<2|base64_dec_tbl[in[1]-43]>>4);
	if(len>1)out[1]=(unsigned char)(base64_dec_tbl[in[1]-43]<<4|base64_dec_tbl[in[2]-43]>>2);
	if(len>2)out[2]=(unsigned char)(((base64_dec_tbl[in[2]-43]<<6)&0xc0)|base64_dec_tbl[in[3]-43]);
}

/*
* calc length of encoded string for input string length of "len"
*/
int base64_enc_len(int len)
{
	return (len+2-((len+2)%3))/3*4;
}

/*
* calc length of decoded string for encoded string "in" length of "len"
*/
int base64_dec_len(char * in,int len)
{
	int i=0;
	int numEq=0;
	for(i=len-1;(in[i]=='='&&i>=0);--i)
		++numEq;
	return (len/4*3)-numEq;
}

void base64_encode(const unsigned char *in,unsigned char *out,int len)
{
	int in_pos=0,out_pos=0;
	int full_blocks=len/3;
	int i=0;
	for(;i<full_blocks;++i)
	{
		base64_encodeblock(in+in_pos,out+out_pos,3);
		in_pos+=3;
		out_pos+=4;
	}
	if(len%3!=0)
		base64_encodeblock(in+in_pos,out+out_pos,len%3);
}

void base64_decode(const unsigned char *in,unsigned char *out,int len)
{
	int in_pos=0,out_pos=0;
	int full_blocks=len/3;
	int i=0;
	for(;i<full_blocks;++i)
	{
		base64_decodeblock(in+in_pos,out+out_pos,3);
		in_pos+=4;
		out_pos+=3;
	}
	if(len%3!=0)
		base64_decodeblock(in+in_pos,out+out_pos,len%3);
}

int indexOf(char c)
{
	int i=0;
	if(c=='=')return 0;
	for(;i<64;++i)
		if(base64_enc_tbl[i]==c)return i;
	return -1;
}

/* TEST
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv)
{
	int i=43;
	int index=0;

	//print char map for base64 syms
	char charMap[122-43+2];
	memset(charMap,0,122-43+2);
	for(;i<123;++i)
		charMap[i-43]=(char)i;
	printf("%s",charMap);

	print decode table
	printf("{");
	for(;i<123;++i)
	{
		index=indexOf(i);
		if(index!=-1)
			printf("0x%02x,",index);
		else printf("0xff,");
		if((i-42)%10==0)printf("\n");
	}
	printf("}");
	printf("\n");

	char s1[200];
	char s2[200];
	memset(s1,0,200);
	memset(s2,0,200);
	strcpy(s1,"Qwertyui");
	printf("str=%s\n",s1);
	printf("strlen=%i\n",strlen(s1));
	printf("enclen=%i\n",base64_enc_len(strlen(s1)));
	base64_encode((unsigned char *)s1,(unsigned char *)s2,strlen(s1));
	printf("declen=%i\n",base64_dec_len(s2,strlen(s2)));
	printf("encoded=%s\n",s2);
	memset(s1,0,200);
	base64_decode((unsigned char *)s2,(unsigned char *)s1,strlen(s2));
	printf("decoded=%s\n",s1);
	return 0;
}*/
