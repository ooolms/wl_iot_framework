import qbs

Module
{
	Depends {name: "cpp"}
	cpp.cFlags:
	{
		if(qbs.buildVariant=="debug")return ["-fprofile-arcs","-ftest-coverage"];
		else return [];
	}
	cpp.cxxFlags:
	{
		if(qbs.buildVariant=="debug")return ["-fprofile-arcs","-ftest-coverage"];
		else return [];
	}
	cpp.staticLibraries:
	{
		if(qbs.buildVariant=="debug")return ["gcov"];
		else return [];
	}
}
