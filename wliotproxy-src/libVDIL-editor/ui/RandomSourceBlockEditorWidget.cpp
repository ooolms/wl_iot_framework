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

#include "RandomSourceBlockEditorWidget.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QScrollArea>
#include <limits>
#include <QApplication>
#include <QDebug>

using namespace WLIOTVDIL;

RandomSourceBlockEditorWidget::RandomSourceBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	QWidget *w1=new QWidget(this);
	QLabel *dimLabel=new QLabel("dim",w1);
	dimEdit=new QSpinBox(w1);
	dimEdit->setRange(1,std::numeric_limits<unsigned short>::max());
	dimEdit->setValue(1);
	scroll=new QScrollArea(this);
	spinsList=new QWidget(scroll);
	scroll->setWidget(spinsList);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(w1);
	mainLayout->addWidget(scroll);

	QHBoxLayout *w1Layout=new QHBoxLayout(w1);
	w1Layout->setContentsMargins(0,0,0,0);
	w1Layout->addWidget(dimLabel);
	w1Layout->addWidget(dimEdit,1);

	new QVBoxLayout(spinsList);

	connect(dimEdit,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this,&RandomSourceBlockEditorWidget::updateSpins);

	updateSpins();
}

void RandomSourceBlockEditorWidget::setParams(const QList<QPair<qint32,qint32>> &bounds)
{
	dimEdit->setValue(qMax(1,bounds.count()));
	updateSpins();
	if(bounds.isEmpty())return;
	for(int i=0;i<bounds.count();++i)
	{
		minEdits[i]->setValue(bounds[i].first);
		maxEdits[i]->setValue(bounds[i].second);
	}
}

QList<QPair<qint32,qint32>> RandomSourceBlockEditorWidget::bounds()const
{
	QList<QPair<qint32,qint32>> r;
	for(int i=0;i<spinsWidgets.count();++i)
	{
		qint32 min=minEdits[i]->value();
		qint32 max=maxEdits[i]->value();
		if(min>=max)
		{
			min=0;
			max=RAND_MAX;
		}
		r.append(qMakePair(min,max));
	}
	return r;
}

void RandomSourceBlockEditorWidget::updateSpins()
{
	int dim=dimEdit->value();
	scroll->takeWidget();
	if(spinsWidgets.count()>dim)
	{
		while(spinsWidgets.count()>dim)
		{
			delete spinsWidgets.last();
			spinsWidgets.removeLast();
			minEdits.removeLast();
			maxEdits.removeLast();
		}
	}
	else if(spinsWidgets.count()<dim)
	{
		while(spinsWidgets.count()<dim)
		{
			QWidget *w=new QWidget(spinsList);
			QSpinBox *minEdit=new QSpinBox(w);
			QSpinBox *maxEdit=new QSpinBox(w);
			QLabel *minLabel=new QLabel("min",w);
			QLabel *maxLabel=new QLabel("max",w);
			QHBoxLayout *l=new QHBoxLayout(w);
			l->setContentsMargins(0,0,0,0);
			l->addWidget(minLabel);
			l->addWidget(minEdit);
			l->addWidget(maxLabel);
			l->addWidget(maxEdit);
			spinsWidgets.append(w);
			minEdits.append(minEdit);
			maxEdits.append(maxEdit);
			minEdit->setRange(std::numeric_limits<int>::min(),std::numeric_limits<int>::max()-1);
			maxEdit->setRange(std::numeric_limits<int>::min()+1,std::numeric_limits<int>::max());
			minEdit->setValue(minEdit->minimum());
			maxEdit->setValue(maxEdit->maximum());
		}
	}

	delete spinsList->layout();
	spinsList->setFixedSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
	QVBoxLayout *lay=new QVBoxLayout(spinsList);
	for(QWidget *w:spinsWidgets)
		lay->addWidget(w);
	spinsList->resize(lay->minimumSize());
	scroll->setWidget(spinsList);
	if(scroll->isVisible())
		spinsList->show();
}
