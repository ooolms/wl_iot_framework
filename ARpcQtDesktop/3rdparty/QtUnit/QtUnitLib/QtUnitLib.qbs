import qbs

StaticLibrary
{
	name: "QtUnitLib"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","gui","widgets","xml"]}

	Export
	{
		Depends {name: "cpp"}
		cpp.includePaths: "."
		Depends {name: "Qt"; submodules: ["core","gui","widgets","xml"]}
	}

	files: [
        "CmdArgParser.cpp",
        "CmdArgParser.h",
        "QtUnitConsoleRunner.cpp",
        "QtUnitConsoleRunner.h",
        "QtUnitMain.cpp",
        "QtUnitMain.h",
        "QtUnitTestCollection.cpp",
        "QtUnitTestCollection.h",
        "QtUnitTestSet.cpp",
        "QtUnitTestSet.h",
        "QtUnitWidget.cpp",
        "QtUnitWidget.h",
        "QtUnitXmlDump.cpp",
        "QtUnitXmlDump.h",
        "Resources/qtunit.qrc",
        "ShowLogDialog.cpp",
        "ShowLogDialog.h",
    ]
}
