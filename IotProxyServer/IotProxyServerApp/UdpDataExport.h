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

#ifndef UDPDATAEXPORT_H
#define UDPDATAEXPORT_H

#include <QUdpSocket>
#include <QMutex>
#include "ARpcBase/ARpcMessage.h"

class UdpDataExport
{
public:
	static void setExportAddress(const QString &address);
	static void writeMeasurement(const QUuid &devId,const QByteArray &sensorName,const QByteArrayList &data);

private:
	static const quint16 exportPort=6081;
	static QUdpSocket socket;
	static bool enabled;
	static QHostAddress exportAddress;
	static QMutex mutex;
};

#endif // UDPDATAEXPORT_H
