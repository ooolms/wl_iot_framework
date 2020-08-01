#include "XmlParserTests.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
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
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/blocks/RuntimeSourceBlock.h"
#include "VDIL/blocks/RuntimeStoreBlock.h"
#include "VDIL/blocks/MathExpBlock.h"
#include "VDIL/core/SubProgramBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

//TODO tests for a different blocks

XmlParserTests::XmlParserTests(QObject *parent)
	:QtUnit::QtUnitTestSet("XmlParserTests",parent)
{
	bf=new BlocksFactory;
	bxpf=new BlocksXmlParserFactory;
	addTest((TestFunction)&XmlParserTests::testParserAllBlocks,
		"check if we didn't forget to add some standart blocks to xml factory");
	addTest((TestFunction)&XmlParserTests::testCommandBlock,"test command block parsing");
	addTest((TestFunction)&XmlParserTests::testComparationBlock,"test comparation block parsing");
	addTest((TestFunction)&XmlParserTests::testDimChangeBlock,"test dim change block parsing");
	addTest((TestFunction)&XmlParserTests::testNormingBlock,"test norming block parsing");
	addTest((TestFunction)&XmlParserTests::testStaticSourceBlock,"test static source block parsing");
	addTest((TestFunction)&XmlParserTests::testStorageSourceBlock,"test storage source block parsing");
	addTest((TestFunction)&XmlParserTests::testDelayBlock,"test delay block parsing");
	addTest((TestFunction)&XmlParserTests::testDebugBlock,"test debug block parsing");
	addTest((TestFunction)&XmlParserTests::testDeviceStateSourceBlock,"test device state source block parsing");
	addTest((TestFunction)&XmlParserTests::testDateTimeSourceBlock,"test date+time source block parsing");
	addTest((TestFunction)&XmlParserTests::testArrayCombineBlock,"test array combine block parsing");
	addTest((TestFunction)&XmlParserTests::testArraySelectBlock,"test array select block parsing");
	addTest((TestFunction)&XmlParserTests::testRandomSourceBlock,"test random source block parsing");
	addTest((TestFunction)&XmlParserTests::testTimerBlock,"test timer block parsing");
	addTest((TestFunction)&XmlParserTests::testDevicePresenceSourceBlock,"test device presence source block parsing");
	addTest((TestFunction)&XmlParserTests::testRuntimeSourceBlock,"test runtime source block parsing");
	addTest((TestFunction)&XmlParserTests::testRuntimeStoreBlock,"test runtime store block parsing");
}

XmlParserTests::~XmlParserTests()
{
	delete bxpf;
	delete bf;
}

void XmlParserTests::testParserAllBlocks()
{
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
	p->addBlock(new DevicePresenceSourceBlock);
	p->addBlock(new RuntimeStoreBlock);
	p->addBlock(new RuntimeSourceBlock);
	SubProgramBlock *sb=new SubProgramBlock;
	p->addBlock(sb);
	sb->subProgram()->addBlock(new AndBoolBlock);
	sb->subProgram()->addBlock(new NormingBlock);

	QByteArray data=ProgramXmlParser::toXml(bxpf,p);
	VERIFY(!data.isEmpty())
	QScopedPointer<Program> ptr2(ProgramXmlParser::fromXml(bxpf,bf,data,false));
	VERIFY(ptr2.data()!=0)

	COMPARE(p->allBlocks().count(),ptr2->allBlocks().count())
	COMPARE(p->selfBlocks().count(),ptr2->selfBlocks().count())
}

void XmlParserTests::testRuntimeVars()
{
	p->runtimeVars()->setupVar("test_var",DataUnit(1.2));

	QByteArray data=ProgramXmlParser::toXml(bxpf,p);
	VERIFY(!data.isEmpty())
	QScopedPointer<Program> ptr2(ProgramXmlParser::fromXml(bxpf,bf,data,false));
	VERIFY(ptr2.data()!=0)

	COMPARE(p->runtimeVars()->allVars(),ptr2->runtimeVars()->allVars())
	VERIFY(ptr2->runtimeVars()->defaultValue("test_var").isValid())
	COMPARE(p->runtimeVars()->defaultValue("test_var"),ptr2->runtimeVars()->defaultValue("test_var"))
}

void XmlParserTests::testVDev()
{
	QList<SensorDef> sensors;
	SensorDef d;
	d.type=SensorDef::Type(SensorDef::F32,SensorDef::PACKET,SensorDef::LOCAL_TIME,3);
	d.name="sens1";
	d.title="sens11";
	d.unit="u1";
	d.attributes["a1"]="b1";
	sensors.append(d);

	ControlsGroup ctl;
	ctl.title="All ctls";
	ControlsCommand cmd;
	cmd.title="Cmd1";
	cmd.layout=Qt::Horizontal;
	cmd.forceBtn=true;
	cmd.buttonText="123";
	cmd.commandToExec="cmd1";
	ControlsCommandParam p1;
	p1.type=ControlsCommandParam::SLIDER;
	p1.title="p1";
	p1.attributes["min"]="1";
	cmd.params.append(p1);
	ctl.elements.append(ControlsGroup::Element(cmd));

	p->vdev()->setSensors(sensors);
	p->vdev()->setControls(ctl);
	p->vdev()->setParams(true,"{5397b7b3-71a6-4f59-a8d5-82fa06815f92}","somename");

	QByteArray data=ProgramXmlParser::toXml(bxpf,p);
	VERIFY(!data.isEmpty())
	QScopedPointer<Program> ptr2(ProgramXmlParser::fromXml(bxpf,bf,data,false));
	VERIFY(ptr2.data()!=0)

	COMPARE(p->vdev()->enabled(),ptr2->vdev()->enabled())
	COMPARE(p->vdev()->devId(),ptr2->vdev()->devId())
	COMPARE(p->vdev()->devName(),ptr2->vdev()->devName())
	COMPARE(p->vdev()->sensors(),ptr2->vdev()->sensors())
	COMPARE(p->vdev()->controls(),ptr2->vdev()->controls())
}

#define BLOCK_TEST_ADD_BLOCK_AND_PARSE \
	p->addBlock(b); \
	QByteArray data=ProgramXmlParser::toXml(bxpf,p); \
	VERIFY(!data.isEmpty()) \
	QScopedPointer<Program> ptr2(ProgramXmlParser::fromXml(bxpf,bf,data,false)); \
	VERIFY(ptr2.data()!=nullptr)

#define BLOCK_TEST_CHECK_BLOCK_TYPE(BlockType) \
	COMPARE(ptr2->allBlocks().count(),1) \
	const BaseBlock *bb2=ptr2->allBlocks().value(b->blockId()); \
	VERIFY(bb2!=nullptr) \
	COMPARE(bb2->blockName(),BlockType::mBlockName) \
	COMPARE(bb2->groupName(),CoreBlocksGroupFactory::mGroupName) \
	const BlockType *b2=(const BlockType*)bb2;

void XmlParserTests::testCommandBlock()
{
	CommandBlock *b=new CommandBlock;
	b->setParams(QUuid::createUuid(),"cmd1",QByteArrayList()<<"1"<<"${2}",3,true);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(CommandBlock)

	COMPARE(b->deviceId(),b2->deviceId())
	COMPARE(b->cmd(),b2->cmd())
	COMPARE(b->args(),b2->args())
	COMPARE(b->argInputsCount(),b2->argInputsCount())
	COMPARE(b->enableConditionInput(),b2->enableConditionInput())
}

void XmlParserTests::testComparationBlock()
{
	ComparationBlock *b=new ComparationBlock;
	b->setParams(ComparationBlock::SPLITTED_INPUT,true,3,ComparationBlock::GTEQ);
	b->setDistValue(DataUnit(10.3));
	b->setV2Value(DataUnit(33.3));

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(ComparationBlock)

	COMPARE(b->outMode(),b2->outMode())
	COMPARE(b->externalV2Input(),b2->externalV2Input())
	COMPARE(b->dimIndex(),b2->dimIndex())
	COMPARE(b->operation(),b2->operation())
	COMPARE(b->distValue(),b2->distValue())
	COMPARE(b->v2Value(),b2->v2Value())
}

void XmlParserTests::testDimChangeBlock()
{
	DimChangeBlock *b=new DimChangeBlock;
	b->setDim(4);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DimChangeBlock)

	COMPARE(b->dim(),b2->dim())
}

void XmlParserTests::testNormingBlock()
{
	NormingBlock *b=new NormingBlock;
	b->setParams(0LL,12,13,16,4,true);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(NormingBlock)

	COMPARE(b->minX(),b2->minX())
	COMPARE(b->maxX(),b2->maxX())
	COMPARE(b->minY(),b2->minY())
	COMPARE(b->maxY(),b2->maxY())
	COMPARE(b->dimIndex(),b2->dimIndex())
	COMPARE(b->forceLimits(),b2->forceLimits())
}

void XmlParserTests::testStaticSourceBlock()
{
	StaticSourceBlock *b=new StaticSourceBlock;
	b->setParams(DataUnit(455LL),true);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(StaticSourceBlock)

	COMPARE(b->value(),b2->value())
	COMPARE(b->configurable(),b2->configurable())
}

void XmlParserTests::testStorageSourceBlock()
{
	StorageSourceBlock *b=new StorageSourceBlock;
	SensorDef::Type t(SensorDef::U16,SensorDef::SINGLE,SensorDef::GLOBAL_TIME,5);
	b->setParams({QUuid::createUuid(),"sens"},t,6,true,false);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(StorageSourceBlock)

	COMPARE(b->storageId(),b2->storageId())
	COMPARE(b->valuesType(),b2->valuesType())
	COMPARE(b->count(),b2->count())
	COMPARE(b->needDevice(),b2->needDevice())
	COMPARE(b->useTrigger(),b2->useTrigger())
}

void XmlParserTests::testDelayBlock()
{
	DelayBlock *b=new DelayBlock;
	b->setDelay(300);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DelayBlock)

	COMPARE(b->delay(),b2->delay())
}

void XmlParserTests::testDebugBlock()
{
	DebugBlock *b=new DebugBlock;
	b->setParams("str111",4);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DebugBlock)

	COMPARE(b->debugString(),b2->debugString())
	COMPARE(b->argInputsCount(),b2->argInputsCount())
}

void XmlParserTests::testDeviceStateSourceBlock()
{
	DeviceStateSourceBlock *b=new DeviceStateSourceBlock;
	b->setParams(QUuid::createUuid(),"keyy",false,true,6);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DeviceStateSourceBlock)

	COMPARE(b->deviceId(),b2->deviceId())
	COMPARE(b->stateKey(),b2->stateKey())
	COMPARE(b->commandState(),b2->commandState())
	COMPARE(b->commandStateIndex(),b2->commandStateIndex())
	COMPARE(b->boolOut(),b2->boolOut())
}

void XmlParserTests::testDateTimeSourceBlock()
{
	DateTimeSourceBlock *b=new DateTimeSourceBlock;
	b->setDateOutputs(DateTimeSourceBlock::MINUTES|DateTimeSourceBlock::MONTH);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DateTimeSourceBlock)

	COMPARE(b->dateOutputs(),b2->dateOutputs())
}

void XmlParserTests::testArrayCombineBlock()
{
	ArrayCombineBlock *b=new ArrayCombineBlock;
	b->setParams(4,6);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(ArrayCombineBlock)

	COMPARE(b->inputsCount(),b2->inputsCount())
	COMPARE(b->dim(),b2->dim())
}

void XmlParserTests::testArraySelectBlock()
{
	ArraySelectBlock *b=new ArraySelectBlock;
	b->setParams(4,6);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(ArraySelectBlock)

	COMPARE(b->from(),b2->from())
	COMPARE(b->count(),b2->count())
}

void XmlParserTests::testRandomSourceBlock()
{
	RandomSourceBlock *b=new RandomSourceBlock;
	b->setBounds(QList<QPair<qint32,qint32>>()<<qMakePair(qint32(15),qint32(123))<<
		qMakePair(qint32(51),qint32(1546)));

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(RandomSourceBlock)

	COMPARE(b->bounds(),b2->bounds())
}

void XmlParserTests::testTimerBlock()
{
	TimerBlock *b=new TimerBlock;
	TimerBlock::TimerConfig cfg;
	cfg.policy=TimerBlock::REGULAR_MIN;
	cfg.startTime=QDateTime(QDateTime::fromMSecsSinceEpoch(12345678000));
	cfg.repeatInterval=5;
	b->setConfig(cfg,true);

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(TimerBlock)

	COMPARE(b->config().policy,b2->config().policy)
	COMPARE(b->config().startTime.toMSecsSinceEpoch(),b2->config().startTime.toMSecsSinceEpoch())
	COMPARE(b->config().repeatInterval,b2->config().repeatInterval)
	COMPARE(b->configurable(),b2->configurable())
}

void XmlParserTests::testDevicePresenceSourceBlock()
{
	DevicePresenceSourceBlock *b=new DevicePresenceSourceBlock;
	b->setDeviceId(QUuid::createUuid());

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(DevicePresenceSourceBlock)

	COMPARE(b->deviceId(),b2->deviceId())
}

void XmlParserTests::testRuntimeSourceBlock()
{
	RuntimeSourceBlock *b=new RuntimeSourceBlock;
	b->setVarName("test_var");

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(RuntimeSourceBlock)

	COMPARE(b->varName(),b2->varName())
}

void XmlParserTests::testRuntimeStoreBlock()
{
	RuntimeStoreBlock *b=new RuntimeStoreBlock;
	b->setVarName("test_var");

	BLOCK_TEST_ADD_BLOCK_AND_PARSE
	BLOCK_TEST_CHECK_BLOCK_TYPE(RuntimeStoreBlock)

	COMPARE(b->varName(),b2->varName())
}

bool XmlParserTests::testInit()
{
	p=new Program;
	return true;
}

void XmlParserTests::testCleanup()
{
	delete p;
}
