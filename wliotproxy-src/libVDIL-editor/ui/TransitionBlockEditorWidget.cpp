#include "TransitionBlockEditorWidget.h"
#include <QLabel>
#include <QLayout>
#include <QSpinBox>

using namespace WLIOTVDIL;

TransitionBlockEditorWidget::TransitionBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	QLabel *l=new QLabel("outputs count",this);
	countEdit=new QSpinBox(this);
	countEdit->setRange(1,255);
	countEdit->setValue(1);

	QHBoxLayout *lay=new QHBoxLayout(this);
	lay->setContentsMargins(0,0,0,0);
	lay->addWidget(l);
	lay->addWidget(countEdit);
}

void TransitionBlockEditorWidget::setCount(quint32 c)
{
	countEdit->setValue(c);
}

quint32 TransitionBlockEditorWidget::count()
{
	return countEdit->value();
}
