#include "ARpcParamTextEdit.h"
#include <QLineEdit>
#include <QLayout>
#include <QLabel>

ARpcParamTextEdit::ARpcParamTextEdit(const ARpcControlParam &p,QObject *parent)
	:ARpcIParamElement(parent)
{
	w=new QWidget;
	QLabel *l=new QLabel(p.title,w);
	edit=new QLineEdit(w);

	//TODO layout from p
	QVBoxLayout *mainLayout=new QVBoxLayout(w);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(l);
	mainLayout->addWidget(edit);
}

QString ARpcParamTextEdit::paramValue()
{
	return edit->text();//TODO fix "\n" and "|"
}

QWidget *ARpcParamTextEdit::widget()
{
	return w;
}
