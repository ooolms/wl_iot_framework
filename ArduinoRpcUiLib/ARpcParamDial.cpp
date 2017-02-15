#include "ARpcParamDial.h"
#include <QDial>
#include <QLayout>
#include <QLabel>

ARpcParamDial::ARpcParamDial(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QDial(w);

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
	connect(edit,&QDial::sliderReleased,this,&ARpcIParamElement::activated);
}

QString ARpcParamDial::paramValue()
{
	return QString::number(edit->value());
}

QWidget* ARpcParamDial::widget()
{
	return w;
}
