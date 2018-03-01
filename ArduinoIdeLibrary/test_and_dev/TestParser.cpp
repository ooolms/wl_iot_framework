#include "TestParser.h"
#include <QByteArray>
#include <QList>
#include <QDebug>
#include "ARpcBase.h"

static const QByteArrayList testMsgs=QByteArrayList()<<
	"cmd|arg1|arg2|arg3\n"<<
	"cmd2|ar\\\\g1|a\\|rg2|ar\\ng3\n"<<
	"cmd|arg1|arg\\x2f\n"<<
	(QByteArray("cmddddd|aaaarrrrgggg")+QByteArray::fromHex("0x00")+"cmd|arg1\n");

static const QByteArrayList testCmds=QByteArrayList()<<
	"cmd"<<
	"cmd2"<<
	"cmd"<<
	"cmd";

static const QList<QByteArrayList> testArgs=QList<QByteArrayList>()<<
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
	virtual void processMessage(char *cmd,char *args[],unsigned char argsCount)override
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

bool testParser()
{
	bool ok=true;
	qDebug()<<"TEST PARSER";
	for(int i=0;i<testMsgs.count();++i)
	{
		qDebug()<<"TEST"<<i<<": "<<testMsgs[i];
		a.prepareToTest(testCmds[i],testArgs[i]);
		a.putData(testMsgs[i].constData(),testMsgs[i].size());
		if(!a.wasMsg||!a.testResult)
		{
			qDebug()<<"FAILED";
			ok=false;
		}
		else qDebug()<<"OK";
	}
	qDebug()<<"";
	return ok;
}
