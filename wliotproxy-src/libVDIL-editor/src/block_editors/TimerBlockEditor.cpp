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

#include "block_editors/TimerBlockEditor.h"
#include "TimerBlockEditorWidget.h"
#include "VDIL/core/TimerBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* TimerBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new TimerBlockEditorWidget(parent);
}

void TimerBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	TimerBlockEditorWidget *w=(TimerBlockEditorWidget*)editingWidget;
	const TimerBlock *b=(const TimerBlock*)block;
	w->setConfig(b->config(),b->configurable());
}

void TimerBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	TimerBlockEditorWidget *w=(TimerBlockEditorWidget*)editingWidget;
	TimerBlock *b=(TimerBlock*)block;
	b->setConfig(w->config(),w->configurable());
}

QPixmap TimerBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/timer.png");
}

QString TimerBlockEditor::description()const
{
	return "Output for external timer triggers";
}

QString TimerBlockEditor::typeName()const
{
	return "timer src";
}

QString TimerBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	QString hint;
	TimerBlock *b=(TimerBlock*)block;
	hint=TimerBlock::configString(b->config());
	if(b->configurable())
		hint+="; configurable";
	return hint;
}

QString TimerBlockEditor::treeName()const
{
	return "Timer";
}
