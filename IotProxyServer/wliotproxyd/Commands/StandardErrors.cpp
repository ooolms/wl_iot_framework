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

#include "StandardErrors.h"

const QByteArray StandardErrors::invalidAgruments="invalid arguments";
const QByteArray StandardErrors::noDeviceFound="device is not found or more than 1 found with equal names: %1";
const QByteArray StandardErrors::deviceNotIdentified="device is not identified";
const QByteArray StandardErrors::cantWriteDevicesConfig="can't write devices configuration";
const QByteArray StandardErrors::sessionNotFound="session not found";
const QByteArray StandardErrors::storageNotFound="storage not found";
const QByteArray StandardErrors::unknownCommand="unknown command";

const QByteArray StandardErrors::someStrangeError="some strange error happened";