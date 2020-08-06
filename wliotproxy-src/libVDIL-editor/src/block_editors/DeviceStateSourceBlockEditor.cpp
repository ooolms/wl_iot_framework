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
#include "VDIL/blocks/DeviceStateSourceBlock.h"
#include "DeviceStateSourceBlockEditorWidget.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* DeviceStateSourceBlockEditor::mkEditingWidget(IEditorHelper *helper,QWidget *parent)
{
	return new DeviceStateSourceBlockEditorWidget(helper,parent);
}

void DeviceStateSourceBlockEditor::loadParamsFromBlock(IEditorHelper *helper,QWidget *editingWidget,const BaseBlock *block)
{
	DeviceStateSourceBlockEditorWidget *w=(DeviceStateSourceBlockEditorWidget*)editingWidget;
	const DeviceStateSourceBlock *b=(const DeviceStateSourceBlock*)block;
	QString devName;
	if(helper&&!b->deviceId().isNull())
		devName=helper->deviceName(b->deviceId());
	w->setParams(b->deviceId(),devName,b->stateKey(),b->commandState(),b->boolOut(),b->commandStateIndex());
}

void DeviceStateSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget, BaseBlock *block)
{
	DeviceStateSourceBlockEditorWidget *w=(DeviceStateSourceBlockEditorWidget*)editingWidget;
	DeviceStateSourceBlock *b=(DeviceStateSourceBlock*)block;
	b->setParams(w->deviceId(),w->stateKey(),w->commandState(),w->boolOut(),w->commandStateIndex());
}

QPixmap DeviceStateSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/device_state_source.png");
}

QString DeviceStateSourceBlockEditor::description()const
{
	return "device state source";
}

QString DeviceStateSourceBlockEditor::typeName()const
{
	return "dev. state src";
}

QString DeviceStateSourceBlockEditor::hint(IEditorHelper *helper,BaseBlock *block)const
{
	DeviceStateSourceBlock *b=(DeviceStateSourceBlock*)block;
	if(b->deviceId().isNull())return "";
	QString hint="device: ";
	hint+=helper->deviceName(b->deviceId());
	hint+=" ("+b->deviceId().toString()+")";
	hint+="; attribute: ";
	if(b->commandState())
		hint+=QString::fromUtf8(b->stateKey())+"|"+QString::number(b->commandStateIndex());
	else hint+="#|"+QString::fromUtf8(b->stateKey());
	if(b->boolOut())
		hint+="; boolean output";
	return hint;
}

QString DeviceStateSourceBlockEditor::treeName() const
{
	return "Device state source";
}

QString DeviceStateSourceBlockEditor::wikiLink() const
{
	return "";
}
