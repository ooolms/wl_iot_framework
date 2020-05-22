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

#include "DataUnitValueEdit.h"
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QScrollBar>
#include <QLayout>

static const int editWidth=80;

using namespace WLIOTGDIL;

DataUnitValueEdit::DataUnitValueEdit(QWidget *parent)
	:QScrollArea(parent)
{
	setDim(1);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void DataUnitValueEdit::setDim(quint32 dim)
{
	QStringList strs;
	for(QLineEdit *e:edits)
	{
		strs.append(e->text());
		delete e;
	}
	for(QLabel *c:commas)
		delete c;
	edits.clear();
	commas.clear();
	contentWidget=new QWidget;
	QHBoxLayout *lay=new QHBoxLayout(contentWidget);
	for(int i=0;i<(int)dim;++i)
	{
		if(i!=0)
		{
			QLabel *l=new QLabel(",",contentWidget);
			commas.append(l);
			lay->addWidget(l);
		}
		QLineEdit *e=new QLineEdit(contentWidget);
		edits.append(e);
		e->setFixedWidth(editWidth);
		lay->addWidget(e);
		if(i<strs.count())
			e->setText(strs[i]);
	}
	setWidget(contentWidget);
}

bool DataUnitValueEdit::canBeS64()const
{
	for(QLineEdit *e:edits)
	{
		QString t=e->text();
		if(t.contains('.')||t.contains('e',Qt::CaseInsensitive))
			return false;
	}
	return true;
}

QVector<double> DataUnitValueEdit::f64Values(bool *ok)
{
	QVector<double> r;
	r.resize(edits.count());
	if(ok)*ok=true;
	bool okI=false;
	for(int i=0;i<edits.count();++i)
	{
		r[i]=edits[i]->text().toDouble(&okI);
		if(ok)*ok=*ok&&okI;
	}
	return r;
}

QVector<qint64> DataUnitValueEdit::s64Values(bool *ok)
{
	QVector<qint64> r;
	r.resize(edits.count());
	if(ok)*ok=true;
	bool okI=false;
	for(int i=0;i<edits.count();++i)
	{
		r[i]=edits[i]->text().toLongLong(&okI);
		if(ok)*ok=*ok&&okI;
	}
	return r;
}

void DataUnitValueEdit::setValues(const QVector<double> &vals)
{
	if(vals.size()!=edits.count())
		return;
	for(int i=0;i<vals.size();++i)
		edits[i]->setText(QString::fromUtf8(QByteArray::number(vals[i])));
}

void DataUnitValueEdit::setValues(const QVector<qint64> &vals)
{
	if(vals.size()!=edits.count())
		return;
	for(int i=0;i<vals.size();++i)
		edits[i]->setText(QString::fromUtf8(QByteArray::number(vals[i])));
}

QSize DataUnitValueEdit::sizeHint()const
{
	int h=20;
	if(!edits.isEmpty())
		h=edits[0]->sizeHint().height()+horizontalScrollBar()->sizeHint().height()+10;
	return QSize(editWidth+10,h);
}

QSize DataUnitValueEdit::minimumSizeHint()const
{
	return sizeHint();
}
