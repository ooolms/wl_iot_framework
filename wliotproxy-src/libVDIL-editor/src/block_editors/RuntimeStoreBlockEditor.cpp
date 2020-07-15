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

#include "block_editors/RuntimeStoreBlockEditor.h"
#include "OneStringVarSelectWidget.h"
#include "VDIL/blocks/RuntimeStoreBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString RuntimeStoreBlockEditor::typeName()const
{
	return "runtime store";
}

QWidget* RuntimeStoreBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new OneStringVarSelectWidget("variable name",parent);
}

void RuntimeStoreBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	const RuntimeStoreBlock *b=(const RuntimeStoreBlock*)block;
	w->setVariants(b->program()->runtimeVars()->allVars(),b->varName());
}

void RuntimeStoreBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	RuntimeStoreBlock *b=(RuntimeStoreBlock*)block;
	b->setVarName(w->selectedVariant());
}

QPixmap RuntimeStoreBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString RuntimeStoreBlockEditor::description()const
{
	return "store block for a runtime variable";
}

QString RuntimeStoreBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	RuntimeStoreBlock *b=(RuntimeStoreBlock*)block;
	return "var. name: "+b->varName();
}

QString RuntimeStoreBlockEditor::treeName()const
{
	return "Runtime var store";
}
