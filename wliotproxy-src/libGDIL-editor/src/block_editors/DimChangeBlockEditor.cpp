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

#include "block_editors/DimChangeBlockEditor.h"
#include "DimChangeBlockEditorWidget.h"
#include "GDIL/blocks/DimChangeBlock.h"

QString DimChangeBlockEditor::typeName()const
{
	return "select 1 dim";
}

QWidget* DimChangeBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new DimChangeBlockEditorWidget(parent);
}

void DimChangeBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	DimChangeBlockEditorWidget *w=(DimChangeBlockEditorWidget*)editingWidget;
	const DimChangeBlock *b=(const DimChangeBlock*)block;
	w->setDim(b->dim());
}

void DimChangeBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	DimChangeBlockEditorWidget *w=(DimChangeBlockEditorWidget*)editingWidget;
	DimChangeBlock *b=(DimChangeBlock*)block;
	b->setDim(w->dim());
}

QPixmap DimChangeBlockEditor::previewImage() const
{
	return QPixmap(":/GDIL/editor/blocks/dim_change.png");
}

QString DimChangeBlockEditor::description() const
{
	return "taking 1 dimension from all input values";
}
