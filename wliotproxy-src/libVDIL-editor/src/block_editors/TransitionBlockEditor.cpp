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

#include "block_editors/TransitionBlockEditor.h"
#include "TransitionBlockEditorWidget.h"
#include "VDIL/blocks/TransitionBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QWidget* TransitionBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new TransitionBlockEditorWidget(parent);
}

void TransitionBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	TransitionBlockEditorWidget *w=(TransitionBlockEditorWidget*)editingWidget;
	const TransitionBlock *b=(const TransitionBlock*)block;
	w->setCount(b->count());
}

void TransitionBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	TransitionBlockEditorWidget *w=(TransitionBlockEditorWidget*)editingWidget;
	TransitionBlock *b=(TransitionBlock*)block;
	b->setCount(w->count());
}

QPixmap TransitionBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString TransitionBlockEditor::description()const
{
	return "Transition block";
}

QString TransitionBlockEditor::typeName()const
{
	return "trans.";
}

QString TransitionBlockEditor::hint(IEditorHelper *,BaseBlock *)const
{
	return QString();
}

QString TransitionBlockEditor::treeName()const
{
	return "Transition";
}

QString TransitionBlockEditor::wikiLink()const
{
	return "https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:transition_block";
}
