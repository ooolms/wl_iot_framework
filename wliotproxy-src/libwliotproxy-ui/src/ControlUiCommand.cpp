#include "ControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

ControlUiCommand::ControlUiCommand(const CommandControl &cmd,QObject *parent)
	:ControlUiElement(parent)
{
	sendCommandOnElementActivation=false;
	command=cmd.command;
	if(cmd.params.count()==0)
	{
		QPushButton *btn=new QPushButton(cmd.title);
		connect(btn,&QPushButton::clicked,this,&ControlUiCommand::onSendCommand);
		w=btn;
		return;
	}
	sendCommandOnElementActivation=(cmd.params.count()==1&&!cmd.forceBtn);
	if(cmd.params.count()>=1&&cmd.params[0].type==ControlParam::TEXT_EDIT)
		sendCommandOnElementActivation=false;
	QGroupBox *g=new QGroupBox;
	g->setTitle(cmd.title);
	g->setAlignment(Qt::AlignHCenter);
	QBoxLayout *layout=new QBoxLayout((cmd.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,g);

	for(int i=0;i<cmd.params.count();++i)
	{
		IParamElement *el=IParamElement::makeWidget(cmd.params[i]);
		el->setParent(this);
		elements.append(el);
		QWidget *elW=el->widget();
		if(elW)
		{
			el->widget()->setParent(g);
			layout->addWidget(el->widget());
		}
		connect(el,&IParamElement::activated,this,&ControlUiCommand::onElementActivated);
	}

	if(!sendCommandOnElementActivation)
	{
		QPushButton *btn=new QPushButton(g);
		if(!cmd.buttonText.isEmpty())
			btn->setText(QString::fromUtf8(cmd.buttonText));
		else if(cmd.params.isEmpty())
			btn->setText(cmd.title);
		else btn->setText("Set");
		connect(btn,&QPushButton::clicked,this,&ControlUiCommand::onSendCommand);
		layout->addWidget(btn);
	}

	w=g;
}

QWidget* ControlUiCommand::widget()
{
	return w;
}

void ControlUiCommand::updateState(const QMap<int,QByteArray> &values)
{
	for(auto i=values.begin();i!=values.end();++i)
	{
		int index=i.key()-1;
		if(index>=0&&index<elements.count())
			elements[index]->setValue(i.value());
	}
}

QString ControlUiCommand::getCommand()const
{
	return command;
}

void ControlUiCommand::onElementActivated()
{
	if(sendCommandOnElementActivation)onSendCommand();
}

void ControlUiCommand::onSendCommand()
{
	QByteArrayList args;
	for(IParamElement *el:elements)
		args.append(el->paramValue());
	emit executeCommand(command,args);
}
