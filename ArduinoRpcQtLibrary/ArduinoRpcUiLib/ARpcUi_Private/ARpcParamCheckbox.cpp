#include "ARpcParamCheckbox.h"
#include <QCheckBox>

ARpcParamCheckbox::ARpcParamCheckbox(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	onValue="1";
	offValue="0";
	if(p.constraints.contains("onValue")&&!p.constraints["onValue"].isEmpty())
		onValue=p.constraints["onValue"];
	if(p.constraints.contains("offValue")&&!p.constraints["offValue"].isEmpty())
		offValue=p.constraints["offValue"];
	check=new QCheckBox(p.title);
	connect(check,&QCheckBox::toggled,this,&ARpcIParamElement::activated);
}

QString ARpcParamCheckbox::paramValue()
{
	if(check->isChecked())return onValue;
	else return offValue;
}

QWidget* ARpcParamCheckbox::widget()
{
	return check;
}

void ARpcParamCheckbox::setValue(const QString &v)
{
	if(v==offValue)check->setChecked(false);
	else if(v==onValue)check->setChecked(true);
}
