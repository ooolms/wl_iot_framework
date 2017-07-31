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

#ifndef ARPCPARAMNULL_H
#define ARPCPARAMNULL_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class ARpcParamNull
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamNull(QObject *parent=0);

private:
	QWidget *w;

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;
	virtual void setValue(const QString &v)override;

private:
	QString value;
};

#endif // ARPCPARAMNULL_H
