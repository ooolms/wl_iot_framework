#include "ARpcControlUiGroup.h"
#include "ARpcControlUiCommand.h"
#include <QGroupBox>
#include <QLayout>

ARpcControlUiGroup::ARpcControlUiGroup(const ARpcControlsGroup &grp,QObject *parent)
	:ARpcControlUiElement(parent)
{
	w=new QGroupBox;
	w->setAlignment(Qt::AlignHCenter);

	QBoxLayout *layout=new QBoxLayout((grp.layout==ARpcControlsGroup::HORIZONTAL)?
		QBoxLayout::LeftToRight:QBoxLayout::TopToBottom,w);

	for(int i=0;i<grp.elements.count();++i)
	{
		if(grp.elements[i].isGroup())
		{
			ARpcControlUiGroup *g=new ARpcControlUiGroup(*grp.elements[i].group(),this);
			elements.append(g);
			g->widget()->setParent(w);
			layout->addWidget(g->widget());
			connect(g,&ARpcControlUiGroup::executeCommand,this,&ARpcControlUiGroup::executeCommand);
		}
		else if(grp.elements[i].isControl())
		{
			ARpcControlUiCommand *c=new ARpcControlUiCommand(*grp.elements[i].control(),this);
			elements.append(c);
			c->widget()->setParent(w);
			layout->addWidget(c->widget());
			connect(c,&ARpcControlUiCommand::executeCommand,this,&ARpcControlUiGroup::executeCommand);
		}
	}
}

QWidget* ARpcControlUiGroup::widget()
{
	return w;
}

