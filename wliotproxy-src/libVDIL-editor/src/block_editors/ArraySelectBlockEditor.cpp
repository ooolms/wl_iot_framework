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

#include "block_editors/ArraySelectBlockEditor.h"
#include "ArraySelectBlockEditorWidget.h"
#include "VDIL/blocks/ArraySelectBlock.h"
#include "VDIL/core/Program.h"
#include "EditorInternalApi.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* ArraySelectBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new ArraySelectBlockEditorWidget(parent);
}

void ArraySelectBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	ArraySelectBlockEditorWidget *w=(ArraySelectBlockEditorWidget*)editingWidget;
	const ArraySelectBlock *b=(const ArraySelectBlock*)block;
	w->setParams(b->from(),b->count());
}

void ArraySelectBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	ArraySelectBlockEditorWidget *w=(ArraySelectBlockEditorWidget*)editingWidget;
	ArraySelectBlock *b=(ArraySelectBlock*)block;
	b->setParams(w->from(),w->count());
}

QPixmap ArraySelectBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString ArraySelectBlockEditor::description()const
{
	return "Select subarray from array: \"count\" elements from \"start index\"";
}

QString ArraySelectBlockEditor::typeName()const
{
	return "array select";
}

QString ArraySelectBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	ArraySelectBlock *b=(ArraySelectBlock*)block;
	QString hint="start from "+QString::number(b->from())+" "+QString::number(b->count())+" elements";
	return hint;
}

QString ArraySelectBlockEditor::treeName()const
{
	return "Array select";
}
