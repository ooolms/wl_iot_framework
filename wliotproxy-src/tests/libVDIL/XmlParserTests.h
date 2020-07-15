#ifndef XMLPARSERTESTS_H
#define XMLPARSERTESTS_H

#include "QtUnitTestSet.h"

namespace WLIOTVDIL
{
	class Program;
	class BlocksFactory;
	class BlocksXmlParserFactory;
}

class XmlParserTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit XmlParserTests(QObject *parent=nullptr);
	virtual ~XmlParserTests();
	virtual bool testInit()override;
	virtual void testCleanup()override;
	void testParserAllBlocks();
	void testRuntimeVars();
	void testVDev();
	void testCommandBlock();
	void testComparationBlock();
	void testDimChangeBlock();
	void testNormingBlock();
	void testStaticSourceBlock();
	void testStorageSourceBlock();
	void testDelayBlock();
	void testDebugBlock();
	void testDeviceStateSourceBlock();
	void testDateTimeSourceBlock();
	void testArrayCombineBlock();
	void testArraySelectBlock();
	void testRandomSourceBlock();
	void testTimerBlock();
	void testDevicePresenceSourceBlock();
	void testRuntimeSourceBlock();
	void testRuntimeStoreBlock();

private:
	WLIOTVDIL::Program *p;
	WLIOTVDIL::BlocksFactory *bf;
	WLIOTVDIL::BlocksXmlParserFactory *bxpf;
};

#endif // XMLPARSERTESTS_H
