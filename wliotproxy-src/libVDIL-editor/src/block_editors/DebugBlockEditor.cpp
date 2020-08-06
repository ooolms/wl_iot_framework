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
#include "VDIL/blocks/DebugBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget *DebugBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new DebugBlockEditorWidget(parent);
}

void DebugBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	const DebugBlock *b=(const DebugBlock*)block;
	w->setParams(b->debugString(),b->argInputsCount());
}

void DebugBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	DebugBlock *b=(DebugBlock*)block;
	b->setParams(w->debugString(),w->inCount());
}

QPixmap DebugBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/debug.png");
}

QString DebugBlockEditor::description()const
{
	return "writing debug message to log";
}

QString DebugBlockEditor::typeName()const
{
	return "debug";
}

QString DebugBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	DebugBlock *b=(DebugBlock*)block;
	return b->debugString();
}

QString DebugBlockEditor::treeName() const
{
	return "Debug output";
}

QString DebugBlockEditor::wikiLink() const
{
	return "https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:debug_block";
}
