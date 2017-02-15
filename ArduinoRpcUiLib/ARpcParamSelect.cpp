#include "ARpcParamSelect.h"
#include <QComboBox>
#include <QLayout>
#include <QLabel>

ARpcParamSelect::ARpcParamSelect(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QComboBox(w);

	QStringList values;
	if(p.constraints.contains("values"))
		values=p.constraints["values"].split(";",QString::SkipEmptyParts);
	for(QString &s:values)s=s.trimmed();
	values.removeAll(QString());
	if(values.isEmpty())values.append("0");
	edit->addItems(values);

	//TODO layout from p
	QHBoxLayout *mainLayout=new QHBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
	connect(edit,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),this,&ARpcIParamElement::activated);

}

QString ARpcParamSelect::paramValue()
{
	return edit->currentText();
}

QWidget* ARpcParamSelect::widget()
{
	return w;
}
