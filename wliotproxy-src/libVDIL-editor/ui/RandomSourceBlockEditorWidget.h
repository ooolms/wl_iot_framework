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

#ifndef RANDOMSOURCEBLOCKEDITORWIDGET_H
#define RANDOMSOURCEBLOCKEDITORWIDGET_H

#include <QWidget>

class QSpinBox;
class QScrollArea;

namespace WLIOTVDIL
{
	class RandomSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit RandomSourceBlockEditorWidget(QWidget *parent=nullptr);
		void setBounds(const QList<QPair<qint32,qint32>> &bounds);
		QList<QPair<qint32,qint32>> bounds()const;

	private slots:
		void updateSpins();

	private:
		QList<QSpinBox*> minEdits,maxEdits;
		QList<QWidget*> spinsWidgets;
		QSpinBox *dimEdit;
		QWidget *spinsList;
		QScrollArea *scroll;
	};
}

#endif // RANDOMSOURCEBLOCKEDITORWIDGET_H
