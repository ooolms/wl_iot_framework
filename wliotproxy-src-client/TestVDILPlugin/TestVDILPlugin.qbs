import qbs

DynamicLibrary
{
	Depends {name: "Qt"; submodules: ["network","serialport","script","xml"]}
	Depends {name: "libVDIL"}

	Group
	{
		name: "Test VDIL plugin"
		fileTagsFilter: product.type
		qbs.install: true
		qbs.installDir: "usr/lib/wliotproxyd/VDIL-plugins"
	}

	files:[
        "TestVDILPlugin.cpp",
        "TestVDILPlugin.h",
    ]
}
