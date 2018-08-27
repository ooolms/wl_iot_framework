#include "ARpcParamRadio.h"
#include <QLayout>
#include <QRadioButton>
#include <QByteArrayList>
#include <QGroupBox>
#include <QVariant>

ARpcParamRadio::ARpcParamRadio(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	QByteArrayList values,titles;
	if(p.attributes.contains("values"))
		values=p.attributes["values"].split('|');
	if(p.attributes.contains("titles"))
		titles=p.attributes["titles"].split('|');
	for(QByteArray &s:values)
		s=s.trimmed();
	values.removeAll(QByteArray());
	if(values.isEmpty())
		values.append("0");
	w=new QGroupBox;
	QVBoxLayout *l=new QVBoxLayout(w);
	bool useTitles=(titles.count()==values.count());
	for(int i=0;i<values.count();++i)
	{
		QRadioButton *btn=new QRadioButton(QString::fromUtf8(useTitles?titles[i]:values[i]),w);
		btn->setProperty("value",values[i]);
		l->addWidget(btn);
		btns.append(btn);
	}
	btns[0]->setChecked(true);
}

QByteArray ARpcParamRadio::paramValue()
{
	for(QRadioButton *b:btns)
	{
		if(b->isChecked())
			return b->property("value").toByteArray();
	}
	return QByteArray();
}

QWidget *ARpcParamRadio::widget()
{
	return w;
}

void ARpcParamRadio::setValue(const QByteArray &v)
{
	for(QRadioButton *b:btns)
		b->setChecked(false);
	for(QRadioButton *b:btns)
	{
		if(b->text().toUtf8()==v)
		{
			b->setChecked(true);
			break;
		}
	}
}
