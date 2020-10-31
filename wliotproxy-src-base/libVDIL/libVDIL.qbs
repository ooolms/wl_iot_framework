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
        "include/VDIL/core/BaseBlock.h",
        "include/VDIL/core/BlockInput.h",
        "include/VDIL/core/BlockOutput.h",
        "include/VDIL/core/BlockPort.h",
        "include/VDIL/core/BlocksFactory.h",
        "include/VDIL/core/DataUnit.h",
        "include/VDIL/core/Engine.h",
        "include/VDIL/core/IBlocksGroupFactory.h",
        "include/VDIL/core/IEngineCallbacks.h",
        "include/VDIL/core/IEngineHelper.h",
        "include/VDIL/core/IProgramRuntime.h",
        "include/VDIL/core/Program.h",
        "include/VDIL/core/ProgramEvalTimers.h",
        "include/VDIL/core/ProgramRuntimeVars.h",
        "include/VDIL/core/ProgramRuntimesFactory.h",
        "include/VDIL/core/ProgramVirtualDevice.h",
        "include/VDIL/core/RuntimeSourceBlock.h",
        "include/VDIL/core/RuntimeStoreBlock.h",
        "include/VDIL/core/SourceBlock.h",
        "include/VDIL/core/SubProgram.h",
        "include/VDIL/core/SubProgramBlock.h",
        "include/VDIL/core/TimerBlock.h",
        "include/VDIL/core/VDevCommandSourceBlock.h",
        "include/VDIL/core/VDevSensorSendBlock.h",
        "include/VDIL/plugins/VDILEnginePlugin.h",
        "include/VDIL/plugins/VDILEnginePluginsLoader.h",
        "include/VDIL/xml/BaseBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/BlocksXmlParserFactory.h",
        "include/VDIL/xml/DataUnitXmlParser.h",
        "include/VDIL/xml/DefaultBlockXmlParser.h",
        "include/VDIL/xml/IBlockXmlParser.h",
        "include/VDIL/xml/IBlocksGroupXmlParserFactory.h",
        "include/VDIL/xml/ProgramXmlParser.h",
        "include/VDIL/xml/RuntimeSourceBlockXmlParser.h",
        "include/VDIL/xml/RuntimeStoreBlockXmlParser.h",
        "include/VDIL/xml/SubProgramInternalBlocksParser.h",
        "include/VDIL/xml/TimerBlockXmlParser.h",
        "include/VDIL/xml/VDevCommandSourceBlockXmlParser.h",
        "include/VDIL/xml/VDevSensorSendBlockXmlParser.h",
        "src/core/BaseBlock.cpp",
        "src/core/BlockInput.cpp",
        "src/core/BlockOutput.cpp",
        "src/core/BlockPort.cpp",
        "src/core/BlocksFactory.cpp",
        "src/core/DataUnit.cpp",
        "src/core/Engine.cpp",
        "src/core/IEngineCallbacks.cpp",
        "src/core/IEngineHelper.cpp",
        "src/core/IProgramRuntime.cpp",
        "src/core/Program.cpp",
        "src/core/ProgramEvalTimers.cpp",
        "src/core/ProgramRuntimeVars.cpp",
        "src/core/ProgramRuntimesFactory.cpp",
        "src/core/ProgramVirtualDevice.cpp",
        "src/core/RuntimeSourceBlock.cpp",
        "src/core/RuntimeStoreBlock.cpp",
        "src/core/SourceBlock.cpp",
        "src/core/SubProgram.cpp",
        "src/core/SubProgramBlock.cpp",
        "src/core/TimerBlock.cpp",
        "src/core/VDevCommandSourceBlock.cpp",
        "src/core/VDevSensorSendBlock.cpp",
        "src/plugins/VDILEnginePlugin.cpp",
        "src/plugins/VDILEnginePluginsLoader.cpp",
        "src/xml/BaseBlocksGroupXmlParserFactory.cpp",
        "src/xml/BlocksXmlParserFactory.cpp",
        "src/xml/DataUnitXmlParser.cpp",
        "src/xml/DefaultBlockXmlParser.cpp",
        "src/xml/ProgramXmlParser.cpp",
        "src/xml/RuntimeSourceBlockXmlParser.cpp",
        "src/xml/RuntimeStoreBlockXmlParser.cpp",
        "src/xml/SubProgramInternalBlocksParser.cpp",
        "src/xml/TimerBlockXmlParser.cpp",
        "src/xml/VDevCommandSourceBlockXmlParser.cpp",
        "src/xml/VDevSensorSendBlockXmlParser.cpp",
    ]
}
