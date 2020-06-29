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

#include "block_editors/ArrayCombineBlockEditor.h"
#include "ArrayCombineBlockEditorWidget.h"
#include "VDIL/blocks/ArrayCombineBlock.h"
#include "VDIL/core/Program.h"
#include "EditorInternalApi.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* ArrayCombineBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new ArrayCombineBlockEditorWidget(parent);
}

void ArrayCombineBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	ArrayCombineBlockEditorWidget *w=(ArrayCombineBlockEditorWidget*)editingWidget;
	const ArrayCombineBlock *b=(const ArrayCombineBlock*)block;
	w->setParams(b->inputsCount(),b->dim());
}

void ArrayCombineBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	ArrayCombineBlockEditorWidget *w=(ArrayCombineBlockEditorWidget*)editingWidget;
	ArrayCombineBlock *b=(ArrayCombineBlock*)block;
	b->setParams(w->inputsCount(),w->dim());
}

QPixmap ArrayCombineBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString ArrayCombineBlockEditor::description()const
{
	return "Combines single array from multiple inputs";
}

QString ArrayCombineBlockEditor::typeName()const
{
	return "array combine";
}

QString ArrayCombineBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	ArrayCombineBlock *b=(ArrayCombineBlock*)block;
	QString hint=QString::number(b->inputsCount())+" of "+QString::number(b->dim())+"-dim. inputs";
	return hint;
}

QString ArrayCombineBlockEditor::treeName()const
{
	return "Array combine";
}
