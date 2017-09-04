import qbs

Module
{
	Depends {name: "cpp"}
	condition: qbs.buildVariant=="debug"
	cpp.cFlags: ["-fprofile-arcs","-ftest-coverage"]
	cpp.cxxFlags: ["-fprofile-arcs","-ftest-coverage"]
	cpp.linkerFlags: ["-fprofile-arcs","-ftest-coverage","-lgcov"]
}
