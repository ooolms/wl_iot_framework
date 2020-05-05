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

#include "block_editors/DebugBlockEditor.h"
#include "DebugBlockEditorWidget.h"
#include "GDIL/blocks/DebugBlock.h"

QWidget *DebugBlockEditor::mkEditingWidget(EditorInternalApi *,QWidget *parent)
{
	return new DebugBlockEditorWidget(parent);
}

void DebugBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	const DebugBlock *b=(const DebugBlock*)block;
	w->setParams(b->debugString(),b->inCount());
}

void DebugBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	DebugBlock *b=(DebugBlock*)block;
	b->setParams(w->debugString(),w->inCount());
}

QPixmap DebugBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/debug.png");
}

QString DebugBlockEditor::description()const
{
	return "writing debug message to log";
}

QString DebugBlockEditor::typeName()const
{
	return "debug block";
}
