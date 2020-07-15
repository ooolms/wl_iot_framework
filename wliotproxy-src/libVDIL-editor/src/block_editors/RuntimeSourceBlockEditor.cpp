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

#include "block_editors/RuntimeSourceBlockEditor.h"
#include "OneStringVarSelectWidget.h"
#include "VDIL/blocks/RuntimeSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramRuntimeVars.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString RuntimeSourceBlockEditor::typeName()const
{
	return "runtime source";
}

QWidget* RuntimeSourceBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new OneStringVarSelectWidget("variable name",parent);
}

void RuntimeSourceBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	const RuntimeSourceBlock *b=(const RuntimeSourceBlock*)block;
	w->setVariants(b->program()->runtimeVars()->allVars(),b->varName());
}

void RuntimeSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	OneStringVarSelectWidget *w=(OneStringVarSelectWidget*)editingWidget;
	RuntimeSourceBlock *b=(RuntimeSourceBlock*)block;
	b->setVarName(w->selectedVariant());
}

QPixmap RuntimeSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString RuntimeSourceBlockEditor::description()const
{
	return "source block for a runtime variable";
}

QString RuntimeSourceBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	RuntimeSourceBlock *b=(RuntimeSourceBlock*)block;
	return "var. name: "+b->varName();
}

QString RuntimeSourceBlockEditor::treeName()const
{
	return "Runtime var source";
}
