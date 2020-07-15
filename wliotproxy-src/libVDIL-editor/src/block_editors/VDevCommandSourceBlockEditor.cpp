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

#include "block_editors/VDevCommandSourceBlockEditor.h"
#include "OneStringVarSelectWidget.h"
#include "VDIL/blocks/VDevCommandSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramVirtualDevice.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString VDevCommandSourceBlockEditor::typeName()const
{
	return "vdev command src.";
}

QWidget* VDevCommandSourceBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new OneStringVarSelectWidget("command",parent);
}

void VDevCommandSourceBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	const VDevCommandSourceBlock *b=(const VDevCommandSourceBlock*)block;
	QStringList l;
	for(const ControlsCommand &c:block->program()->vdev()->commandsMap())
		l.append(QString::fromUtf8(c.commandToExec));
	w->setVariants(l,QString::fromUtf8(b->command()));
}

void VDevCommandSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	VDevCommandSourceBlock *b=(VDevCommandSourceBlock*)block;
	b->setCommand(w->selectedVariant().toUtf8());
}

QPixmap VDevCommandSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString VDevCommandSourceBlockEditor::description()const
{
	return "provides arguments from executed command";
}

QString VDevCommandSourceBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	VDevCommandSourceBlock *b=(VDevCommandSourceBlock*)block;
	return "command: "+QString::fromUtf8(b->command());
}

QString VDevCommandSourceBlockEditor::treeName()const
{
	return "v.dev. command source";
}
