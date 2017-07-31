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

#ifndef ARPCIPARAMELEMENT_H
#define ARPCIPARAMELEMENT_H

#include "ARpcBase/ARpcControlsDefinition.h"
#include <QWidget>

class ARpcIParamElement
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcIParamElement(QObject *parent=0);
	virtual QString paramValue()=0;
	virtual QWidget* widget()=0;
	virtual void setValue(const QString &v)=0;

public:
	static ARpcIParamElement* makeWidget(const ARpcControlParam &param);

signals:
	void activated();
};

#endif // ARPCIPARAMELEMENT_H
