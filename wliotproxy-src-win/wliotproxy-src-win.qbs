import qbs

Project
{
	references: [
		"../wliotproxy-src/libwliotproxy-base",
		"../wliotproxy-src/libwliotproxy-ui",
		"../wliotproxy-src/libwliotproxy-ui-config",
		"../wliotproxy-src/libwliotproxy",
		"../wliotproxy-src/libwliotproxyd",
		"../wliotproxy-src/wliotproxy-ui-gen",
		"../wliotproxy-src/wliotproxy-ui",
		"../wliotproxy-src/ExampleClientApplication",
		"../wliotproxy-src/TestClientApplication",
		"../wliotproxy-src/3rdparty/QtUnit",
		"../wliotproxy-src/tests",
		"../wliotproxy-src/libVDIL"
	]
}
