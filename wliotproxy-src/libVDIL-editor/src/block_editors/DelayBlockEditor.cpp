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

#include "block_editors/DelayBlockEditor.h"
#include "DelayBlockEditorWidget.h"
#include "VDIL/blocks/DelayBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* DelayBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new DelayBlockEditorWidget(parent);
}

void DelayBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	DelayBlockEditorWidget *w=(DelayBlockEditorWidget*)editingWidget;
	const DelayBlock *b=(const DelayBlock*)block;
	w->setDelay(b->delay());
}

void DelayBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DelayBlockEditorWidget *w=(DelayBlockEditorWidget*)editingWidget;
	DelayBlock *b=(DelayBlock*)block;
	b->setDelay(w->delay());
}

QPixmap DelayBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/delay.png");
}

QString DelayBlockEditor::description()const
{
	return "adding delay to program execution";
}

QString DelayBlockEditor::typeName()const
{
	return "delay";
}

QString DelayBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	QString hint;
	DelayBlock *b=(DelayBlock*)block;
	return "delay for "+QString::number(b->delay())+" milliseconds";
}
