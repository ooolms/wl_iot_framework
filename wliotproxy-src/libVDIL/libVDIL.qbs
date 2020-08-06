import qbs

StaticLibrary
{
	targetName: "VDIL"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","xml"]}
	Depends {name: "libwliotproxy-base"}
	cpp.includePaths: "./include"
	cpp.defines:
	{
		var r=["QT_RESTRICTED_CAST_FROM_ASCII"];
		if(qbs.buildVariant=="debug")
			r.push("DEBUG");
		return r;
	}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","xml"]}
		Depends {name: "libwliotproxy-base"}
		cpp.includePaths: "./include"
	}

	files:[
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
        "include/VDIL/blocks/RuntimeSourceBlock.h",
        "include/VDIL/blocks/RuntimeStoreBlock.h",
        "include/VDIL/blocks/StaticSourceBlock.h",
        "include/VDIL/blocks/StorageSourceBlock.h",
        "include/VDIL/blocks/TransitionBlock.h",
        "include/VDIL/blocks/VDevCommandSourceBlock.h",
        "include/VDIL/blocks/VDevSensorSendBlock.h",
        "include/VDIL/core/BaseBlock.h",
        "include/VDIL/core/BlockInput.h",
        "include/VDIL/core/BlockOutput.h",
        "include/VDIL/core/BlockPort.h",
        "include/VDIL/core/BlocksFactory.h",
        "include/VDIL/core/CoreBlocksGroupFactory.h",
        "include/VDIL/core/DataUnit.h",
        "include/VDIL/core/IBlocksGroupFactory.h",
        "include/VDIL/core/IEngineCallbacks.h",
        "include/VDIL/core/IEngineHelper.h",
        "include/VDIL/core/LogicalBlocksGroupFactory.h",
        "include/VDIL/core/Program.h",
        "include/VDIL/core/ProgramDevicesBridge.h",
        "include/VDIL/core/ProgramEvalTimers.h",
        "include/VDIL/core/ProgramObject.h",
        "include/VDIL/core/ProgramRuntimeVars.h",
        "include/VDIL/core/ProgramStoragesBridge.h",
        "include/VDIL/core/ProgramVirtualDevice.h",
        "include/VDIL/core/SourceBlock.h",
        "include/VDIL/core/SubProgram.h",
        "include/VDIL/core/SubProgramBlock.h",
        "include/VDIL/core/TimerBlock.h",
        "include/VDIL/support/MathExp.h",
        "include/VDIL/support/MathExpFunctions.h",
        "include/VDIL/xml/ArrayCombineBlockXmlParser.h",
        "include/VDIL/xml/ArraySelectBlockXmlParser.h",
        "include/VDIL/xml/BaseBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/BlocksXmlParserFactory.h",
        "include/VDIL/xml/CommandBlockXmlParser.h",
        "include/VDIL/xml/ComparationBlockXmlParser.h",
        "include/VDIL/xml/CoreBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/DataUnitXmlParser.h",
        "include/VDIL/xml/DateTimeSourceBlockXmlParser.h",
        "include/VDIL/xml/DebugBlockXmlParser.h",
        "include/VDIL/xml/DefaultBlockXmlParser.h",
        "include/VDIL/xml/DelayBlockXmlParser.h",
        "include/VDIL/xml/DevicePresenceSourceBlockXmlParser.h",
        "include/VDIL/xml/DeviceStateSourceBlockXmlParser.h",
        "include/VDIL/xml/DimSelectBlockXmlParser.h",
        "include/VDIL/xml/IBlockXmlParser.h",
        "include/VDIL/xml/IBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/LinearTransformationBlockXmlParser.h",
        "include/VDIL/xml/LogicalBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/MathExpBlockXmlParser.h",
        "include/VDIL/xml/ProgramXmlParser.h",
        "include/VDIL/xml/RandomSourceBlockXmlParser.h",
        "include/VDIL/xml/RuntimeSourceBlockXmlParser.h",
        "include/VDIL/xml/RuntimeStoreBlockXmlParser.h",
        "include/VDIL/xml/StaticSourceBlockXmlParser.h",
        "include/VDIL/xml/StorageSourceBlockXmlParser.h",
        "include/VDIL/xml/TimerBlockXmlParser.h",
        "include/VDIL/xml/TransitionBlockXmlParser.h",
        "include/VDIL/xml/VDevCommandSourceBlockXmlParser.h",
        "include/VDIL/xml/VDevSensorSendBlockXmlParser.h",
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
        "src/blocks/RuntimeSourceBlock.cpp",
        "src/blocks/RuntimeStoreBlock.cpp",
        "src/blocks/StaticSourceBlock.cpp",
        "src/blocks/StorageSourceBlock.cpp",
        "src/blocks/TransitionBlock.cpp",
        "src/blocks/VDevCommandSourceBlock.cpp",
        "src/blocks/VDevSensorSendBlock.cpp",
        "src/core/BaseBlock.cpp",
        "src/core/BlockInput.cpp",
        "src/core/BlockOutput.cpp",
        "src/core/BlockPort.cpp",
        "src/core/BlocksFactory.cpp",
        "src/core/CoreBlocksGroupFactory.cpp",
        "src/core/DataUnit.cpp",
        "src/core/IEngineCallbacks.cpp",
        "src/core/IEngineHelper.cpp",
        "src/core/LogicalBlocksGroupFactory.cpp",
        "src/core/Program.cpp",
        "src/core/ProgramDevicesBridge.cpp",
        "src/core/ProgramEvalTimers.cpp",
        "src/core/ProgramObject.cpp",
        "src/core/ProgramRuntimeVars.cpp",
        "src/core/ProgramStoragesBridge.cpp",
        "src/core/ProgramVirtualDevice.cpp",
        "src/core/SourceBlock.cpp",
        "src/core/SubProgram.cpp",
        "src/core/SubProgramBlock.cpp",
        "src/core/TimerBlock.cpp",
        "src/xml/ArrayCombineBlockXmlParser.cpp",
        "src/xml/ArraySelectBlockXmlParser.cpp",
        "src/xml/BaseBlocksGroupXmlParserFactory.cpp",
        "src/xml/BlocksXmlParserFactory.cpp",
        "src/xml/CommandBlockXmlParser.cpp",
        "src/xml/ComparationBlockXmlParser.cpp",
        "src/xml/CoreBlocksGroupXmlParserFactory.cpp",
        "src/xml/DataUnitXmlParser.cpp",
        "src/xml/DateTimeSourceBlockXmlParser.cpp",
        "src/xml/DebugBlockXmlParser.cpp",
        "src/xml/DefaultBlockXmlParser.cpp",
        "src/xml/DelayBlockXmlParser.cpp",
        "src/xml/DevicePresenceSourceBlockXmlParser.cpp",
        "src/xml/DeviceStateSourceBlockXmlParser.cpp",
        "src/xml/DimSelectBlockXmlParser.cpp",
        "src/xml/LinearTransformationBlockXmlParser.cpp",
        "src/xml/LogicalBlocksGroupXmlParserFactory.cpp",
        "src/xml/MathExpBlockXmlParser.cpp",
        "src/xml/ProgramXmlParser.cpp",
        "src/xml/RandomSourceBlockXmlParser.cpp",
        "src/xml/RuntimeSourceBlockXmlParser.cpp",
        "src/xml/RuntimeStoreBlockXmlParser.cpp",
        "src/xml/StaticSourceBlockXmlParser.cpp",
        "src/xml/StorageSourceBlockXmlParser.cpp",
        "src/xml/TimerBlockXmlParser.cpp",
        "src/xml/TransitionBlockXmlParser.cpp",
        "src/xml/VDevCommandSourceBlockXmlParser.cpp",
        "src/xml/VDevSensorSendBlockXmlParser.cpp",
    ]
}
