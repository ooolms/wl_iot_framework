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

#ifndef LASTNVALUESINMEMORYSTORAGE_H
#define LASTNVALUESINMEMORYSTORAGE_H

#include "BaseFSSensorStorage.h"

namespace WLIOT
{
	class LastNValuesInMemoryStorage
		:public BaseFSSensorStorage
	{
		Q_OBJECT

	public:
		explicit LastNValuesInMemoryStorage(const QString &path,const QUuid &devId,const QByteArray &devName,
			const SensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
		virtual ~LastNValuesInMemoryStorage();
		bool create(quint32 storedValuesCount);
		virtual void close()override;

	public:
		virtual bool open()override;
		virtual bool isOpened()const override;
		virtual quint64 valuesCount()override;
		virtual SensorValue* valueAt(quint64 index)override;
		virtual bool writeSensorValue(const SensorValue *val)override;

	private:
		void resizeValues(quint32 sz);

	private:
		QList<QByteArray> values;
		bool opened;
	};
}

#endif // LASTNVALUESINMEMORYSTORAGE_H
