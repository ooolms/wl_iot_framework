import qbs

StaticLibrary
{
	targetName: "VDIL-core-blocks"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","xml"]}
	Depends {name: "libwliotproxy-base"}
	Depends {name: "libVDIL"}
	cpp.includePaths: "./include"
	cpp.defines:
	{
		var r=["QT_RESTRICTED_CAST_FROM_ASCII"];
		if(qbs.buildVariant=="debug")
			r.push("DEBUG_BUILD");
		return r;
	}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","xml"]}
		Depends {name: "libwliotproxy-base"}
		Depends {name: "libVDIL"}
		cpp.includePaths: "./include"
	}

	files:[
        "include/VDIL/CoreBlocksPlugin.h",
        "include/VDIL/blocks/ArrayCombineBlock.h",
        "include/VDIL/blocks/ArraySelectBlock.h",
        "include/VDIL/blocks/AverageCalcBlock.h",
        "include/VDIL/blocks/CommandBlock.h",
        "include/VDIL/blocks/ComparationBlock.h",
        "include/VDIL/blocks/ConditionTransitionBlock.h",
        "include/VDIL/blocks/DateTimeSourceBlock.h",
        "include/VDIL/blocks/DebugBlock.h",
        "include/VDIL/blocks/DelayBlock.h",
        "include/VDIL/blocks/DevicePresenceSourceBlock.h",
        "include/VDIL/blocks/DeviceStateSourceBlock.h",
        "include/VDIL/blocks/DimSelectBlock.h",
        "include/VDIL/blocks/LinearTransformationBlock.h",
        "include/VDIL/blocks/LogicalBlocks.h",
        "include/VDIL/blocks/MathExpBlock.h",
        "include/VDIL/blocks/RandomSourceBlock.h",
        "include/VDIL/blocks/StaticSourceBlock.h",
        "include/VDIL/blocks/StorageSourceBlock.h",
        "include/VDIL/blocks/TransitionBlock.h",
        "include/VDIL/core/CoreBlocksGroupFactory.h",
        "include/VDIL/core/LogicalBlocksGroupFactory.h",
        "include/VDIL/core/ProgramDevicesRuntime.h",
        "include/VDIL/core/ProgramStoragesRuntime.h",
        "include/VDIL/support/MathExp.h",
        "include/VDIL/support/MathExpFunctions.h",
        "include/VDIL/xml/ArrayCombineBlockXmlParser.h",
        "include/VDIL/xml/ArraySelectBlockXmlParser.h",
        "include/VDIL/xml/CommandBlockXmlParser.h",
        "include/VDIL/xml/ComparationBlockXmlParser.h",
        "include/VDIL/xml/CoreBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/DateTimeSourceBlockXmlParser.h",
        "include/VDIL/xml/DebugBlockXmlParser.h",
        "include/VDIL/xml/DelayBlockXmlParser.h",
        "include/VDIL/xml/DevicePresenceSourceBlockXmlParser.h",
        "include/VDIL/xml/DeviceStateSourceBlockXmlParser.h",
        "include/VDIL/xml/DimSelectBlockXmlParser.h",
        "include/VDIL/xml/LinearTransformationBlockXmlParser.h",
        "include/VDIL/xml/LogicalBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/MathExpBlockXmlParser.h",
        "include/VDIL/xml/RandomSourceBlockXmlParser.h",
        "include/VDIL/xml/StaticSourceBlockXmlParser.h",
        "include/VDIL/xml/StorageSourceBlockXmlParser.h",
        "include/VDIL/xml/TransitionBlockXmlParser.h",
        "src/CoreBlocksPlugin.cpp",
        "src/MathExp.cpp",
        "src/MathExpFunctions.cpp",
        "src/blocks/ArrayCombineBlock.cpp",
        "src/blocks/ArraySelectBlock.cpp",
        "src/blocks/AverageCalcBlock.cpp",
        "src/blocks/CommandBlock.cpp",
        "src/blocks/ComparationBlock.cpp",
        "src/blocks/ConditionTransitionBlock.cpp",
        "src/blocks/DateTimeSourceBlock.cpp",
        "src/blocks/DebugBlock.cpp",
        "src/blocks/DelayBlock.cpp",
        "src/blocks/DevicePresenceSourceBlock.cpp",
        "src/blocks/DeviceStateSourceBlock.cpp",
        "src/blocks/DimSelectBlock.cpp",
        "src/blocks/LinearTransformationBlock.cpp",
        "src/blocks/LogicalBlocks.cpp",
        "src/blocks/MathExpBlock.cpp",
        "src/blocks/RandomSourceBlock.cpp",
        "src/blocks/StaticSourceBlock.cpp",
        "src/blocks/StorageSourceBlock.cpp",
        "src/blocks/TransitionBlock.cpp",
        "src/core/CoreBlocksGroupFactory.cpp",
        "src/core/LogicalBlocksGroupFactory.cpp",
        "src/core/ProgramDevicesRuntime.cpp",
        "src/core/ProgramStoragesRuntime.cpp",
        "src/xml/ArrayCombineBlockXmlParser.cpp",
        "src/xml/ArraySelectBlockXmlParser.cpp",
        "src/xml/CommandBlockXmlParser.cpp",
        "src/xml/ComparationBlockXmlParser.cpp",
        "src/xml/CoreBlocksGroupXmlParserFactory.cpp",
        "src/xml/DateTimeSourceBlockXmlParser.cpp",
        "src/xml/DebugBlockXmlParser.cpp",
        "src/xml/DelayBlockXmlParser.cpp",
        "src/xml/DevicePresenceSourceBlockXmlParser.cpp",
        "src/xml/DeviceStateSourceBlockXmlParser.cpp",
        "src/xml/DimSelectBlockXmlParser.cpp",
        "src/xml/LinearTransformationBlockXmlParser.cpp",
        "src/xml/LogicalBlocksGroupXmlParserFactory.cpp",
        "src/xml/MathExpBlockXmlParser.cpp",
        "src/xml/RandomSourceBlockXmlParser.cpp",
        "src/xml/StaticSourceBlockXmlParser.cpp",
        "src/xml/StorageSourceBlockXmlParser.cpp",
        "src/xml/TransitionBlockXmlParser.cpp",
    ]
}
