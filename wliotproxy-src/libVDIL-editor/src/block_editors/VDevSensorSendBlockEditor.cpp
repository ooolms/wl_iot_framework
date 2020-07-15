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

#include "block_editors/VDevSensorSendBlockEditor.h"
#include "OneStringVarSelectWidget.h"
#include "VDIL/blocks/VDevSensorSendBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramVirtualDevice.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString VDevSensorSendBlockEditor::typeName()const
{
	return "vdev sensor";
}

QWidget* VDevSensorSendBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new OneStringVarSelectWidget("sensor name",parent);
}

void VDevSensorSendBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	const VDevSensorSendBlock *b=(const VDevSensorSendBlock*)block;
	QStringList l;
	for(const SensorDef &d:block->program()->vdev()->sensors())
		l.append(QString::fromUtf8(d.name));
	w->setVariants(l,QString::fromUtf8(b->sensorName()));
}

void VDevSensorSendBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	VDevSensorSendBlock *b=(VDevSensorSendBlock*)block;
	b->setSensorName(w->selectedVariant().toUtf8());
}

QPixmap VDevSensorSendBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString VDevSensorSendBlockEditor::description()const
{
	return "send value as virtual device sensor measurement";
}

QString VDevSensorSendBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	VDevSensorSendBlock *b=(VDevSensorSendBlock*)block;
	return "sensor name: "+QString::fromUtf8(b->sensorName());
}

QString VDevSensorSendBlockEditor::treeName()const
{
	return "v.dev. sensor";
}
