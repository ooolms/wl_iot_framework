#include "ARpcControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>

ARpcControlUiCommand::ARpcControlUiCommand(const ARpcControlsCommand &cmd,QObject *parent)
	:ARpcControlUiElement(parent)
{
	syncCall=cmd.syncCall;
	sendCommandOnElementActivation=(cmd.params.count()==1);
	if(cmd.params.count()==1&&cmd.params[0].type==ARpcControlParam::TEXT_EDIT)
		sendCommandOnElementActivation=false;
	command=cmd.command;
	w=new QGroupBox;
	w->setTitle(cmd.title);
	w->setAlignment(Qt::AlignHCenter);
	//TODO layout for command
	QBoxLayout *layout=new QBoxLayout(QBoxLayout::TopToBottom,w);

	for(int i=0;i<cmd.params.count();++i)
	{
		ARpcIParamElement *el=ARpcIParamElement::makeWidget(cmd.params[i]);
		el->setParent(this);
		elements.append(el);
		el->widget()->setParent(w);
		layout->addWidget(el->widget());
		connect(el,&ARpcIParamElement::activated,this,&ARpcControlUiCommand::onElementActivated);
	}

	if(!sendCommandOnElementActivation)
	{
		QPushButton *btn=new QPushButton(w);
		if(cmd.params.isEmpty())btn->setText(cmd.title);
		else btn->setText("Send");
		connect(btn,&QPushButton::clicked,this,&ARpcControlUiCommand::onSendCommand);
		layout->addWidget(btn);
	}
}

QWidget* ARpcControlUiCommand::widget()
{
	return w;
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
