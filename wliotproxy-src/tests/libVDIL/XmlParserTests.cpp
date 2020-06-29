#include "XmlParserTests.h"
#include "VDIL/blocks/AverageCalcBlock.h"
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/blocks/ComparationBlock.h"
#include "VDIL/blocks/DimChangeBlock.h"
#include "VDIL/blocks/ConditionTransactionBlock.h"
#include "VDIL/blocks/NormingBlock.h"
#include "VDIL/blocks/StaticSourceBlock.h"
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/blocks/DebugBlock.h"
#include "VDIL/blocks/DelayBlock.h"
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "VDIL/blocks/DateTimeSourceBlock.h"
#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/blocks/RandomSourceBlock.h"
#include "VDIL/core/TimerBlock.h"
#include "VDIL/blocks/LogicalBlocks.h"
#include "VDIL/core/Program.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDIL/core/BlocksFactory.h"

using namespace WLIOTVDIL;

XmlParserTests::XmlParserTests(QObject *parent)
	:QtUnit::QtUnitTestSet("XmlParserTests",parent)
{
	addTest((TestFunction)&XmlParserTests::mainTest,"1 big test");
}

void XmlParserTests::mainTest()
{
	QScopedPointer<Program> ptr(new Program);
	Program *p=ptr.data();
	StaticSourceBlock *stSrc=new StaticSourceBlock;
	DebugBlock *dbg=new DebugBlock();
	p->addBlock(stSrc);
	p->addBlock(dbg);
	stSrc->output(0)->linkTo(dbg->input(0));
	p->addBlock(new AverageCalcBlock);
	p->addBlock(new CommandBlock);
	p->addBlock(new ComparationBlock);
	p->addBlock(new DimChangeBlock);
	p->addBlock(new ConditionTransactionBlock);
	p->addBlock(new NormingBlock);
	p->addBlock(new StorageSourceBlock);
	p->addBlock(new DelayBlock);
	p->addBlock(new DeviceStateSourceBlock);
	p->addBlock(new DateTimeSourceBlock);
	p->addBlock(new ArrayCombineBlock);
	p->addBlock(new ArraySelectBlock);
	p->addBlock(new RandomSourceBlock);
	p->addBlock(new TimerBlock);
	p->addBlock(new NotBoolBlock);
	p->addBlock(new AndBoolBlock);
	p->addBlock(new OrBoolBlock);
	p->addBlock(new XorBoolBlock);
	p->addBlock(new AndNotBoolBlock);
	p->addBlock(new OrNotBoolBlock);
	p->addBlock(new XorNotBoolBlock);

	BlocksFactory bf;
	BlocksXmlParserFactory bxpf;
	QByteArray data=ProgramXmlParser::toXml(&bxpf,p);
	VERIFY(!data.isEmpty())
	QScopedPointer<Program> ptr2(new Program);
	Program *p2=ProgramXmlParser::fromXml(&bxpf,&bf,data,false);
	VERIFY(p2!=0)
	delete p2;
}
