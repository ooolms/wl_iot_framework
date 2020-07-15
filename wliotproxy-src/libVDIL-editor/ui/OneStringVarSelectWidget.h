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

#ifndef ONESTRINGVARSELECTWIDGET_H
#define ONESTRINGVARSELECTWIDGET_H

#include <QWidget>

class QComboBox;

namespace WLIOTVDIL
{
	class OneStringVarSelectWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit OneStringVarSelectWidget(const QString &labelText,QWidget *parent=nullptr);
		void setVariants(const QStringList &allVariants,const QString &currentVariant);
		QString selectedVariant();

	private:
		QComboBox *mSelect;
	};
}

#endif // ONESTRINGVARSELECTWIDGET_H
