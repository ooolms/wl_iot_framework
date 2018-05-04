import qbs

CppApplication
{
    name: "QtUnitApp"
    Depends {name: "QtUnitLib"}
	cpp.cxxFlags:{
		if(qbs.buildVariant=="debug")return ["-fprofile-arcs","-ftest-coverage","-pg"]
		return [];
	}

	cpp.staticLibraries:{
		if(qbs.buildVariant=="debug")return ["gcov"]
		return [];
	}

    files: [
        "main.cpp",
        "test2qtunittest.cpp",
        "test2qtunittest.h",
        "testqtunittest.cpp",
        "testqtunittest.h",
    ]
}
