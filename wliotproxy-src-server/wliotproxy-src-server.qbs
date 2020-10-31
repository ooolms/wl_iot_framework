/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

import qbs

Project
{
	references:[
		"../wliotproxy-src-base/libwliotproxy-base",
		"../wliotproxy-src-base/libwliotproxyd",
		"../wliotproxy-src-base/libVDIL",
		"../wliotproxy-src-base/libVDIL-core-blocks",
		"../wliotproxy-src-client/libwliotproxy-static",
		"libwliotproxy-ui",
		"libwliotproxy-ui-config",
		"wliotproxy-ui-gen",
		"wliotproxyd",
		"wliotproxy",
		"wliotproxy-ui",
		"wliotproxy-setup-device",
		"wliotproxy-run-vdil",
		"wliotproxy-run-js",
	]
}
