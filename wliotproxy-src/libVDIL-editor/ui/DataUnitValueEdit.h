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

#ifndef DATAUNITVALUEEDIT_H
#define DATAUNITVALUEEDIT_H

#include <QScrollArea>

class QLineEdit;
class QLabel;

namespace WLIOTVDIL
{
	class DataUnitValueEdit
		:public QScrollArea
	{
		Q_OBJECT
	public:
		explicit DataUnitValueEdit(QWidget *parent=nullptr);
		void setDim(quint32 dim);
		bool canBeS64()const;
		QVector<double> f64Values(bool *ok=0);
		QVector<qint64> s64Values(bool *ok=0);
		void setValues(const QVector<double> &vals);
		void setValues(const QVector<qint64> &vals);
		virtual QSize sizeHint()const override;
		virtual QSize minimumSizeHint()const override;

	private:
		QWidget *contentWidget;
		QList<QLineEdit*> edits;
		QList<QLabel*> commas;
	};
}

#endif // DATAUNITVALUEEDIT_H
