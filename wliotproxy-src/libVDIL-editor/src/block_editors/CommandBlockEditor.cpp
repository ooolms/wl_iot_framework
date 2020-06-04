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
#include "VDIL/blocks/CommandBlock.h"
#include "VDIL/core/Program.h"
#include "EditorInternalApi.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* CommandBlockEditor::mkEditingWidget(IEditorHelper *helper,QWidget *parent)
{
	return new CommandBlockEditorWidget(helper,parent);
}

void CommandBlockEditor::loadParamsFromBlock(IEditorHelper *helper,QWidget *editingWidget,const BaseBlock *block)
{
	CommandBlockEditorWidget *w=(CommandBlockEditorWidget*)editingWidget;
	const CommandBlock *b=(const CommandBlock*)block;
	QString devName;
	if(helper&&!b->deviceId().isNull())
		devName=helper->deviceName(b->deviceId());
	w->setParams(b->deviceId(),devName,b->cmd(),b->args(),b->inCount(),b->enableConditionInput());
}

void CommandBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	CommandBlockEditorWidget *w=(CommandBlockEditorWidget*)editingWidget;
	CommandBlock *b=(CommandBlock*)block;
	b->setParams(w->devId(),w->cmd(),w->args(),w->inCount(),w->enableConditionInput());
}

QPixmap CommandBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/command.png");
}

QString CommandBlockEditor::description()const
{
	return "executing command on the device";
}

QString CommandBlockEditor::typeName()const
{
	return "device command";
}

QString CommandBlockEditor::hint(IEditorHelper *helper,BaseBlock *block)const
{
	CommandBlock *b=(CommandBlock*)block;
	if(b->deviceId().isNull())
		return "";
	QString hint="device: ";
	hint+=helper->deviceName(b->deviceId());
	hint+=" ("+b->deviceId().toString()+")";
	hint+="\ncmd: "+QString::fromUtf8(b->cmd())+"\nargs: "+QString::fromUtf8(b->args().join("|"));
	return hint;
}
