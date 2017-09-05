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

#ifndef STANDARDERRORS_H
#define STANDARDERRORS_H

#include <QString>

class StandardErrors
{
public:
	static const QString invalidAgruments;
	static const QString noDeviceWithId;
	static const QString deviceNotIdentified;
	static const QString cantWriteDevicesConfig;
};

#endif // STANDARDERRORS_H
