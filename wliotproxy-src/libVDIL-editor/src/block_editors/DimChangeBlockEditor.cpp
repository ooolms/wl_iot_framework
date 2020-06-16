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
#include "VDIL/blocks/DimChangeBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString DimChangeBlockEditor::typeName()const
{
	return "select dim";
}

QWidget* DimChangeBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new DimChangeBlockEditorWidget(parent);
}

void DimChangeBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	DimChangeBlockEditorWidget *w=(DimChangeBlockEditorWidget*)editingWidget;
	const DimChangeBlock *b=(const DimChangeBlock*)block;
	w->setDim(b->dim());
}

void DimChangeBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DimChangeBlockEditorWidget *w=(DimChangeBlockEditorWidget*)editingWidget;
	DimChangeBlock *b=(DimChangeBlock*)block;
	b->setDim(w->dim());
}

QPixmap DimChangeBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/dim_change.png");
}

QString DimChangeBlockEditor::description()const
{
	return "taking 1 dimension from all input values";
}

QString DimChangeBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	DimChangeBlock *b=(DimChangeBlock*)block;
	return QString::fromUtf8("select dim "+QByteArray::number(b->dim()));
}

QString DimChangeBlockEditor::treeName() const
{
	return "1 dim select";
}
