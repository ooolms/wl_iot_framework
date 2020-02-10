#include "ControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

ControlUiCommand::ControlUiCommand(const CommandControl &cmd,QObject *parent)
	:ControlUiElement(parent)
{
	sendCommandOnElementActivation=false;
	command=cmd.command;
	int numOfVisibleParams=this->numOfVisibleParams(cmd);
	int indexOfFirstVisibleParam=this->indexOfFirstVisibleParam(cmd);
	sendCommandOnElementActivation=(numOfVisibleParams==1&&!cmd.forceBtn&&
		cmd.params[indexOfFirstVisibleParam].type!=ControlParam::TEXT_EDIT);

	for(int i=0;i<cmd.params.count();++i)
	{
		IParamElement *el=IParamElement::makeElement(cmd.params[i]);
		el->setParent(this);
		elements.append(el);
		connect(el,&IParamElement::activated,this,&ControlUiCommand::onElementActivated);
	}

	if(numOfVisibleParams==0)
	{
		QPushButton *btn=new QPushButton(QString::fromUtf8(cmd.title));
		connect(btn,&QPushButton::clicked,this,&ControlUiCommand::onSendCommand);
		w=btn;
	}
	else if(sendCommandOnElementActivation)
		w=elements[indexOfFirstVisibleParam]->widget();
	else
	{
		if(cmd.title.isEmpty())
			w=new QWidget;
		else
		{
			QGroupBox *g=new QGroupBox;
			g->setTitle(QString::fromUtf8(cmd.title));
			g->setAlignment(Qt::AlignHCenter);
			w=g;
		}

		QBoxLayout *layout=new QBoxLayout((cmd.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,w);

		for(int i=0;i<elements.count();++i)
		{
			QWidget *elW=elements[i]->widget();
			if(elW)
			{
				elW->setParent(w);
				layout->addWidget(elW);
			}
		}

		QPushButton *btn=new QPushButton(w);
		if(!cmd.buttonText.isEmpty())
			btn->setText(QString::fromUtf8(cmd.buttonText));
		else if(cmd.params.isEmpty())
			btn->setText(QString::fromUtf8(cmd.title));
		else btn->setText(cmd.title.isEmpty()?"Send":QString::fromUtf8(cmd.title));
		connect(btn,&QPushButton::clicked,this,&ControlUiCommand::onSendCommand);
		layout->addWidget(btn);
	}
}

QWidget* ControlUiCommand::widget()
{
	return w;
}

void ControlUiCommand::updateState(const QMap<quint32,QByteArray> &values)
{
	for(auto i=values.begin();i!=values.end();++i)
	{
		if(i.key()<elements.count())
			elements[i.key()]->setValue(i.value());
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

int ControlUiCommand::numOfVisibleParams(const CommandControl &cmd)
{
	int c=0;
	for(int i=0;i<cmd.params.count();++i)
		if(cmd.params[i].type!=ControlParam::BAD_TYPE&&cmd.params[i].type!=ControlParam::HIDDEN)
			++c;
	return c;
}

int ControlUiCommand::indexOfFirstVisibleParam(const CommandControl &cmd)
{
	for(int i=0;i<cmd.params.count();++i)
		if(cmd.params[i].type!=ControlParam::BAD_TYPE&&cmd.params[i].type!=ControlParam::HIDDEN)
			return i;
	return -1;
}
