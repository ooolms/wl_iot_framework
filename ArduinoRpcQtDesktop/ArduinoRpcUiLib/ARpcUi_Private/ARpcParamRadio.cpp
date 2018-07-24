#include "ARpcParamRadio.h"
#include <QLayout>
#include <QRadioButton>
#include <QByteArrayList>
#include <QGroupBox>

ARpcParamRadio::ARpcParamRadio(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	QByteArrayList values;
	if(p.attributes.contains("values"))
		values=p.attributes["values"].split('|');
	for(QByteArray &s:values)
		s=s.trimmed();
	values.removeAll(QByteArray());
	if(values.isEmpty())w=0;
	else
	{
		w=new QGroupBox;
		QVBoxLayout *l=new QVBoxLayout(w);
		for(const QByteArray &s:values)
		{
			QRadioButton *btn=new QRadioButton(QString::fromUtf8(s),w);
			l->addWidget(btn);
			btns.append(btn);
		}
		btns[0]->setChecked(true);
	}
}

QByteArray ARpcParamRadio::paramValue()
{
	for(QRadioButton *b:btns)
	{
		if(b->isChecked())
			return b->text().toUtf8();
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
