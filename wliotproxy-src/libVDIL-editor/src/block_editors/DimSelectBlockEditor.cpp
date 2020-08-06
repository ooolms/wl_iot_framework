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

#include "block_editors/DimSelectBlockEditor.h"
#include "DimSelectBlockEditorWidget.h"
#include "VDIL/blocks/DimSelectBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString DimSelectBlockEditor::typeName()const
{
	return "select dim";
}

QWidget* DimSelectBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new DimSelectBlockEditorWidget(parent);
}

void DimSelectBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	DimSelectBlockEditorWidget *w=(DimSelectBlockEditorWidget*)editingWidget;
	const DimSelectBlock *b=(const DimSelectBlock*)block;
	w->setDim(b->dim());
}

void DimSelectBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DimSelectBlockEditorWidget *w=(DimSelectBlockEditorWidget*)editingWidget;
	DimSelectBlock *b=(DimSelectBlock*)block;
	b->setDim(w->dim());
}

QPixmap DimSelectBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/dim_change.png");
}

QString DimSelectBlockEditor::description()const
{
	return "taking 1 dimension from all input values";
}

QString DimSelectBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	DimSelectBlock *b=(DimSelectBlock*)block;
	return QString::fromUtf8("select dim "+QByteArray::number(b->dim()));
}

QString DimSelectBlockEditor::treeName() const
{
	return "1 dim select";
}

QString DimSelectBlockEditor::wikiLink() const
{
	return "https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:1_dim_select_block";
}
