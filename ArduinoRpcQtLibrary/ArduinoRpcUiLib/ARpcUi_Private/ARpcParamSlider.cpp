#include "ARpcParamSlider.h"
#include <QSlider>
#include <QLayout>
#include <QLabel>

ARpcParamSlider::ARpcParamSlider(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QSlider(Qt::Horizontal,w);

	bool ok=false;
	int v;
	if(p.constraints.contains("min"))
		v=p.constraints["min"].toInt(&ok);
	if(ok)edit->setMinimum(v);
	if(p.constraints.contains("max"))
		v=p.constraints["max"].toInt(&ok);
	if(ok)edit->setMaximum(v);
	if(p.constraints.contains("step"))
		v=p.constraints["step"].toInt(&ok);
	if(ok)edit->setSingleStep(v);

	//TODO layout from p
	QVBoxLayout *mainLayout=new QVBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
	connect(edit,&QSlider::sliderReleased,this,&ARpcIParamElement::activated);
}

QString ARpcParamSlider::paramValue()
{
	return QString::number(edit->value());
}

QWidget* ARpcParamSlider::widget()
{
	return w;
}

void ARpcParamSlider::setValue(const QString &v)
{
	bool ok=false;
	int iv=v.toInt(&ok);
	if(!ok)return;
	edit->setValue(iv);
}
