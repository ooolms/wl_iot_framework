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

#ifndef ARPCDBDRIVERGTIMEINDEX_H
#define ARPCDBDRIVERGTIMEINDEX_H

#include <QObject>
#include "ARpcStorages/ARpcDBDriverFixedBlocks.h"
#include "ARpcStorages/VeryBigArray.h"

class ARpcDBDriverGTimeIndex
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDBDriverGTimeIndex(QObject *parent=0);
	virtual ~ARpcDBDriverGTimeIndex();
	bool create(const QString &filePath);
	bool open(const QString &filePath);
	bool isOpened();
	void close();
	bool append(qint64 tm,quint64 dbInd);
	quint64 findIndex(qint64 ts);

private:
	void loadIndex();

private:
	ARpcDBDriverFixedBlocks *dbDriver;
	VeryBigArray<qint64> times;
	VeryBigArray<quint64> dbIndexes;
	bool opened;
};

#endif // ARPCDBDRIVERGTIMEINDEX_H
