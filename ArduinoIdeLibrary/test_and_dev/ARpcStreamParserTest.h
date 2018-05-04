#ifndef ARPCSTREAMPARSERTEST_H
#define ARPCSTREAMPARSERTEST_H

#include "QtUnitTestSet.h"
#include "ARpcIMessageCallback.h"
#include "ARpcStreamParser.h"

class ARpcParserTest
	:public ARpcIMessageCallback
{
public:
	explicit ARpcParserTest(unsigned long bSize)
		:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
	{
		testResult=false;
		wasMsg=false;
	}

	void prepareToTest(const QByteArray &cmd,const QByteArrayList args)
	{
		testCmd=cmd;
		testArgs=args;
	}

	virtual void processMessage(const char *cmd,const char *args[],unsigned char argsCount)override
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
	ARpcStreamParser parser;
	QByteArray testCmd;
	QByteArrayList testArgs;
	bool testResult;
	bool wasMsg;
};

class ARpcStreamParserTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcStreamParserTest(QObject *parent=nullptr);
	void testSimple();
	void testEscapedSyms();
	void testHexSeq();
	void testReset();
	void testParserBufferOverflow();

private:
	ARpcParserTest testObj;
};

#endif // ARPCSTREAMPARSERTEST_H
