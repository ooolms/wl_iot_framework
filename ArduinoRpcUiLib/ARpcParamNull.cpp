#include "ARpcParamNull.h"

ARpcParamNull::ARpcParamNull(QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
}

QString ARpcParamNull::paramValue()
{
	return "null";
}

QWidget* ARpcParamNull::widget()
{
	return w;
}
