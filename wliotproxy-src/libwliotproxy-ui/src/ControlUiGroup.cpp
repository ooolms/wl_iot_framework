/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ControlUiGroup.h"
#include "ControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>

using namespace WLIOTUi;
using namespace WLIOT;

ControlUiGroup::ControlUiGroup(const WLIOT::ControlsGroup &grp,QObject *parent)
	:ControlUiElement(parent)
{
	w=new QGroupBox;
	w->setAlignment(Qt::AlignHCenter);
	QFont f=w->font();
	f.setBold(true);
	w->setFont(f);
	w->setTitle(grp.title);

	QBoxLayout *layout=new QBoxLayout((grp.layout==Qt::Horizontal)?QBoxLayout::LeftToRight:QBoxLayout::TopToBottom,w);
	layout->setContentsMargins(0,0,0,0);

	for(int i=0;i<grp.elements.count();++i)
	{
		if(grp.elements[i].isGroup())
		{
			ControlUiGroup *g=new ControlUiGroup(*grp.elements[i].group(),this);
			elements.append(g);
			g->widget()->setParent(w);
			layout->addWidget(g->widget());
			connect(g,&ControlUiGroup::executeCommand,this,&ControlUiGroup::executeCommand);
		}
		else if(grp.elements[i].isCommand())
		{
			ControlUiCommand *c=new ControlUiCommand(*grp.elements[i].command(),this);
			elements.append(c);
			c->widget()->setParent(w);
			layout->addWidget(c->widget());
			connect(c,&ControlUiCommand::executeCommand,this,&ControlUiGroup::executeCommand);
		}
	}
}

QWidget* ControlUiGroup::widget()
{
	return w;
}

void ControlUiGroup::updateState(const DeviceState &state)
{
	for(int i=0;i<elements.count();++i)
	{
		if(elements[i]->isGroup())
		{
			ControlUiGroup *g=(ControlUiGroup*)elements[i];
			g->updateState(state);
		}
		else if(elements[i]->isCommand())
		{
			ControlUiCommand *c=(ControlUiCommand*)elements[i];
			QByteArray cmd=c->getCommand().toUtf8();
			if(state.commandParams.contains(cmd))
				c->updateState(state.commandParams[cmd]);
		}
	}
}

