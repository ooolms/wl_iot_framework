#include <QCoreApplication>
#include "ARpcBase.h"
#include <stdio.h>
#include <QDebug>

const QByteArrayList testMsgs=QByteArrayList()<<
	"cmd|arg1|arg2|arg3\n"<<
	"cmd2|ar\\\\g1|a\\|rg2|ar\\ng3\n"<<
	"cmd|arg1|arg\\x2f\n"<<
	(QByteArray("cmddddd|aaaarrrrgggg")+QByteArray::fromHex("0x00")+"cmd|arg1\n");

const QByteArrayList testCmds=QByteArrayList()<<
	"cmd"<<
	"cmd2"<<
	"cmd"<<
	"cmd";

const QList<QByteArrayList> testArgs=QList<QByteArrayList>()<<
	(QByteArrayList()<<"arg1"<<"arg2"<<"arg3")<<
	(QByteArrayList()<<"ar\\g1"<<"a|rg2"<<"ar\ng3")<<
	(QByteArrayList()<<"arg1"<<"arg/")<<
	(QByteArrayList()<<"arg1");

class ARpcParserTest
	:public ARpcBase
{
public:
	ARpcParserTest(unsigned long bSize,ARpcWriteCallback wcb)
		:ARpcBase(bSize,wcb)
	{
		testResult=false;
		wasMsg=false;
	}

	void prepareToTest(const QByteArray &cmd,const QByteArrayList args)
	{
		testCmd=cmd;
		testArgs=args;
	}

protected:
	virtual void processMessage(char *cmd,char *args[],int argsCount)override
	{
		wasMsg=true;
		testResult=true;
		if(testArgs.count()!=argsCount)goto fail_mark;
		if(testCmd!=cmd)goto fail_mark;
		for(int i=0;i<argsCount;++i)
			if(testArgs[i]!=args[i])goto fail_mark;
		return;
fail_mark:
		testResult=false;
	}

public:
	QByteArray testCmd;
	QByteArrayList testArgs;
	bool testResult;
	bool wasMsg;
};

void writeCallback(const char *data,unsigned long sz)
{
	fwrite(data,sz,1,stdout);
}

ARpcParserTest a(300,&writeCallback);

void writeToParser(const QByteArray &msg)
{
	for(int i=0;i<msg.size();++i)
		a.putChar(msg[i]);
}

int main(int argc,char *argv[])
{
	bool fails=false;
	for(int i=0;i<testMsgs.count();++i)
	{
		qDebug()<<"TEST"<<i<<": "<<testMsgs[i];
		a.prepareToTest(testCmds[i],testArgs[i]);
		writeToParser(testMsgs[i]);
		if(!a.wasMsg||!a.testResult)
		{
			qDebug()<<"FAILED";
			fails=true;
		}
		else qDebug()<<"OK";
	}
	return fails?1:0;
}
