#include "ARpcParamNull.h"

ARpcParamNull::ARpcParamNull(QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	value="null";
}

QString ARpcParamNull::paramValue()
{
	return value;
}

QWidget* ARpcParamNull::widget()
{
	return w;
}

void ARpcParamNull::setValue(const QString &v)
{
	value=v;
}
