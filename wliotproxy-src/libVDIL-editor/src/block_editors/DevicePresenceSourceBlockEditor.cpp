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

#include "block_editors/DevicePresenceSourceBlockEditor.h"
#include "DevicePresenceSourceBlockEditorWidget.h"
#include "VDIL/blocks/DevicePresenceSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* DevicePresenceSourceBlockEditor::mkEditingWidget(IEditorHelper *helper,QWidget *parent)
{
	return new DevicePresenceSourceBlockEditorWidget(helper,parent);
}

void DevicePresenceSourceBlockEditor::loadParamsFromBlock(
	IEditorHelper *helper,QWidget *editingWidget,const BaseBlock *block)
{
	DevicePresenceSourceBlockEditorWidget *w=(DevicePresenceSourceBlockEditorWidget*)editingWidget;
	const DevicePresenceSourceBlock *b=(const DevicePresenceSourceBlock*)block;
	QString devName;
	if(helper&&!b->deviceId().isNull())
		devName=helper->deviceName(b->deviceId());
	w->setParams(b->deviceId(),devName);
}

void DevicePresenceSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DevicePresenceSourceBlockEditorWidget *w=(DevicePresenceSourceBlockEditorWidget*)editingWidget;
	DevicePresenceSourceBlock *b=(DevicePresenceSourceBlock*)block;
	b->setDeviceId(w->deviceId());
}

QPixmap DevicePresenceSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString DevicePresenceSourceBlockEditor::description()const
{
	return "source block and corresponding trigger to check device presence";
}

QString DevicePresenceSourceBlockEditor::typeName()const
{
	return "device_presence";
}

QString DevicePresenceSourceBlockEditor::hint(IEditorHelper *helper,BaseBlock *block)const
{
	QString hint;
	DevicePresenceSourceBlock *b=(DevicePresenceSourceBlock*)block;
	hint+=helper->deviceName(b->deviceId());
	hint+=" ("+b->deviceId().toString()+")";
	return hint;
}

QString DevicePresenceSourceBlockEditor::treeName()const
{
	return "Device presence source";
}

QString DevicePresenceSourceBlockEditor::wikiLink() const
{
	return "https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:device_presence_source_block";
}
