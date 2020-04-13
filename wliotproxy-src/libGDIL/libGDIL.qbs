import qbs

StaticLibrary
{
	targetName: "GDIL"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","xml"]}
	Depends {name: "libwliotproxy-base"}
	cpp.includePaths: "./include"

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","xml"]}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "./include"
	}

	files:[
        "include/GDIL/blocks/AllBlocks.h",
        "include/GDIL/blocks/AverageCalcBlock.h",
        "include/GDIL/blocks/CommandBlock.h",
        "include/GDIL/blocks/ComparationBlock.h",
        "include/GDIL/blocks/ConditionTransactionBlock.h",
        "include/GDIL/blocks/DebugBlock.h",
        "include/GDIL/blocks/DeviceStateSourceBlock.h",
        "include/GDIL/blocks/DimChangeBlock.h",
        "include/GDIL/blocks/LogicalBlocks.h",
        "include/GDIL/blocks/NormingBlock.h",
        "include/GDIL/blocks/StaticSourceBlock.h",
        "include/GDIL/blocks/StorageSourceBlock.h",
        "include/GDIL/core/BaseBlock.h",
        "include/GDIL/core/BlockInput.h",
        "include/GDIL/core/BlockOutput.h",
        "include/GDIL/core/BlockPort.h",
        "include/GDIL/core/BlocksFactory.h",
        "include/GDIL/core/CoreBlocksGroupFactory.h",
        "include/GDIL/core/DataUnit.h",
        "include/GDIL/core/Engine.h",
        "include/GDIL/core/IBlocksGroupFactory.h",
        "include/GDIL/core/IEngineCallbacks.h",
        "include/GDIL/core/IEngineHelper.h",
        "include/GDIL/core/LogicalBlocksGroupFactory.h",
        "include/GDIL/core/Program.h",
        "include/GDIL/core/ProgramObject.h",
        "include/GDIL/core/ProgramThread.h",
        "include/GDIL/core/SourceBlock.h",
        "include/GDIL/xml/BaseBlocksGroupXmlParserFactory.h",
        "include/GDIL/xml/BlocksXmlParserFactory.h",
        "include/GDIL/xml/CommandBlockXmlParser.h",
        "include/GDIL/xml/ComparationBlockXmlParser.h",
        "include/GDIL/xml/CoreBlocksGroupXmlParserFactory.h",
        "include/GDIL/xml/DataUnitXmlParser.h",
        "include/GDIL/xml/DebugBlockXmlParser.h",
        "include/GDIL/xml/DefaultBlockXmlParser.h",
        "include/GDIL/xml/DimChangeBlockXmlParser.h",
        "include/GDIL/xml/IBlockXmlParser.h",
        "include/GDIL/xml/IBlocksGroupXmlParserFactory.h",
        "include/GDIL/xml/LogicalBlocksGroupXmlParserFactory.h",
        "include/GDIL/xml/NormingBlockXmlParser.h",
        "include/GDIL/xml/ProgramXmlParser.h",
        "include/GDIL/xml/StaticSourceBlockXmlParser.h",
        "include/GDIL/xml/StorageSourceBlockXmlParser.h",
        "src/blocks/AverageCalcBlock.cpp",
        "src/blocks/CommandBlock.cpp",
        "src/blocks/ComparationBlock.cpp",
        "src/blocks/ConditionTransactionBlock.cpp",
        "src/blocks/DebugBlock.cpp",
        "src/blocks/DeviceStateSourceBlock.cpp",
        "src/blocks/DimChangeBlock.cpp",
        "src/blocks/LogicalBlocks.cpp",
        "src/blocks/NormingBlock.cpp",
        "src/blocks/StaticSourceBlock.cpp",
        "src/blocks/StorageSourceBlock.cpp",
        "src/core/BaseBlock.cpp",
        "src/core/BlockInput.cpp",
        "src/core/BlockOutput.cpp",
        "src/core/BlockPort.cpp",
        "src/core/BlocksFactory.cpp",
        "src/core/CoreBlocksGroupFactory.cpp",
        "src/core/DataUnit.cpp",
        "src/core/Engine.cpp",
        "src/core/IEngineCallbacks.cpp",
        "src/core/IEngineHelper.cpp",
        "src/core/LogicalBlocksGroupFactory.cpp",
        "src/core/Program.cpp",
        "src/core/ProgramObject.cpp",
        "src/core/ProgramThread.cpp",
        "src/core/SourceBlock.cpp",
        "src/xml/BaseBlocksGroupXmlParserFactory.cpp",
        "src/xml/BlocksXmlParserFactory.cpp",
        "src/xml/CommandBlockXmlParser.cpp",
        "src/xml/ComparationBlockXmlParser.cpp",
        "src/xml/CoreBlocksGroupXmlParserFactory.cpp",
        "src/xml/DataUnitXmlParser.cpp",
        "src/xml/DebugBlockXmlParser.cpp",
        "src/xml/DefaultBlockXmlParser.cpp",
        "src/xml/DimChangeBlockXmlParser.cpp",
        "src/xml/LogicalBlocksGroupXmlParserFactory.cpp",
        "src/xml/NormingBlockXmlParser.cpp",
        "src/xml/ProgramXmlParser.cpp",
        "src/xml/StaticSourceBlockXmlParser.cpp",
        "src/xml/StorageSourceBlockXmlParser.cpp",
    ]
}
