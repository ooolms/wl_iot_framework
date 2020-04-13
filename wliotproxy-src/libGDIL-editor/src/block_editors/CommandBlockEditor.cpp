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

#include "block_editors/CommandBlockEditor.h"
#include "CommandBlockEditorWidget.h"
#include "GDIL/blocks/CommandBlock.h"

QWidget* CommandBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new CommandBlockEditorWidget(editor,parent);
}

void CommandBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	CommandBlockEditorWidget *w=(CommandBlockEditorWidget*)editingWidget;
	const CommandBlock *b=(const CommandBlock*)block;
	w->setParams(b->devId(),b->devName(),b->cmd(),b->args(),b->inCount());
}

void CommandBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	CommandBlockEditorWidget *w=(CommandBlockEditorWidget*)editingWidget;
	CommandBlock *b=(CommandBlock*)block;
	b->setParams(w->devId(),w->devName(),w->cmd(),w->args(),w->inCount());
}

QPixmap CommandBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/command.png");
}

QString CommandBlockEditor::description()const
{
	return "executing command on the device";
}

QString CommandBlockEditor::typeName()const
{
	return "device command";
}
