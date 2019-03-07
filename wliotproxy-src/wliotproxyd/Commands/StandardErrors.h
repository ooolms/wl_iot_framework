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
	static const QByteArray invalidAgruments;
	static const QByteArray noDeviceFound;
	static const QByteArray deviceNotIdentified;
	static const QByteArray cantWriteDevicesConfig;
	static const QByteArray sessionNotFound;
	static const QByteArray storageNotFound;
	static const QByteArray unknownCommand;
	static const QByteArray noUserFound;
	static const QByteArray accessDenied;

	static const QByteArray someStrangeError;
};

#endif // STANDARDERRORS_H
