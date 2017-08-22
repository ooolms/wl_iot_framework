#include "ARpcControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

ARpcControlUiCommand::ARpcControlUiCommand(const ARpcCommandControl &cmd,QObject *parent)
	:ARpcControlUiElement(parent)
{
	syncCall=cmd.syncCall;
	command=cmd.command;
	if(cmd.params.count()==0)
	{
		QPushButton *btn=new QPushButton(cmd.title);
		connect(btn,&QPushButton::clicked,this,&ARpcControlUiCommand::onSendCommand);
		w=btn;
		return;
	}
	sendCommandOnElementActivation=(cmd.params.count()==1&&!cmd.forceBtn);
	if(cmd.params.count()==1&&cmd.params[0].type==ARpcControlParam::TEXT_EDIT)
		sendCommandOnElementActivation=false;
	QGroupBox *g=new QGroupBox;
	g->setTitle(cmd.title);
	g->setAlignment(Qt::AlignHCenter);
	QBoxLayout *layout=new QBoxLayout((cmd.layout==Qt::Vertical)?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight,g);

	for(int i=0;i<cmd.params.count();++i)
	{
		ARpcIParamElement *el=ARpcIParamElement::makeWidget(cmd.params[i]);
		el->setParent(this);
		elements.append(el);
		el->widget()->setParent(g);
		layout->addWidget(el->widget());
		connect(el,&ARpcIParamElement::activated,this,&ARpcControlUiCommand::onElementActivated);
	}

	if(!sendCommandOnElementActivation)
	{
		QPushButton *btn=new QPushButton(g);
		if(cmd.params.isEmpty())btn->setText(cmd.title);
		else btn->setText("Send");
		connect(btn,&QPushButton::clicked,this,&ARpcControlUiCommand::onSendCommand);
		layout->addWidget(btn);
	}

	w=g;
}

QWidget* ARpcControlUiCommand::widget()
{
	return w;
}

void ARpcControlUiCommand::updateState(const QMap<int,QString> &values)
{
	for(auto i=values.begin();i!=values.end();++i)
	{
		int index=i.key()-1;
		if(index>=0&&index<elements.count())
			elements[index]->setValue(i.value());
	}
}

QString ARpcControlUiCommand::getCommand()const
{
	return command;
}

void ARpcControlUiCommand::onElementActivated()
{
	if(sendCommandOnElementActivation)onSendCommand();
}

void ARpcControlUiCommand::onSendCommand()
{
	QStringList args;
	for(ARpcIParamElement *el:elements)args.append(el->paramValue());
	emit executeCommand(command,args,syncCall);
}
