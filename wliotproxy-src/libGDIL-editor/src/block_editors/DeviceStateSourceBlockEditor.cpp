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

#include "block_editors/DeviceStateSourceBlockEditor.h"
#include "GDIL/blocks/DeviceStateSourceBlock.h"
#include "DeviceStateSourceBlockEditorWidget.h"
#include "GDIL/core/Program.h"

QWidget* DeviceStateSourceBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new DeviceStateSourceBlockEditorWidget(editor,parent);
}

void DeviceStateSourceBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	DeviceStateSourceBlockEditorWidget *w=(DeviceStateSourceBlockEditorWidget*)editingWidget;
	const DeviceStateSourceBlock *b=(const DeviceStateSourceBlock*)block;
	QString devName;
	if(b->program()&&!b->deviceId().isNull())
		devName=b->program()->findDevName(b->deviceId());
	w->setParams(b->deviceId(),devName,b->stateKey(),b->commandState(),b->boolOut(),b->commandStateIndex());
}

void DeviceStateSourceBlockEditor::saveParamsToBlock(QWidget *editingWidget, BaseBlock *block)
{
	DeviceStateSourceBlockEditorWidget *w=(DeviceStateSourceBlockEditorWidget*)editingWidget;
	DeviceStateSourceBlock *b=(DeviceStateSourceBlock*)block;
	b->setParams(w->deviceId(),w->stateKey(),w->commandState(),w->boolOut(),w->commandStateIndex());
}

QPixmap DeviceStateSourceBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/device_state_source.png");
}

QString DeviceStateSourceBlockEditor::description()const
{
	return "device state source";
}

QString DeviceStateSourceBlockEditor::typeName()const
{
	return "device state source";
}
