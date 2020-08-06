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

#include "block_editors/MathExpBlockEditor.h"
#include "MathExpBlockEditorWidget.h"
#include "VDIL/blocks/MathExpBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString MathExpBlockEditor::typeName()const
{
	return "math expression";
}

QWidget* MathExpBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new MathExpBlockEditorWidget(parent);
}

void MathExpBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	MathExpBlockEditorWidget *w=(MathExpBlockEditorWidget*)editingWidget;
	const MathExpBlock *b=(const MathExpBlock*)block;
	w->setParams(b->vars(),b->expr());
}

void MathExpBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	MathExpBlockEditorWidget *w=(MathExpBlockEditorWidget*)editingWidget;
	MathExpBlock *b=(MathExpBlock*)block;
	b->setParams(w->vars(),w->expr());
}

QPixmap MathExpBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString MathExpBlockEditor::description()const
{
	return "evaluating math expression with selected inputs";
}

QString MathExpBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	MathExpBlock *b=(MathExpBlock*)block;
	return b->expr();
}

QString MathExpBlockEditor::treeName()const
{
	return "math expr.";
}

QString MathExpBlockEditor::wikiLink() const
{
	return "https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:math_exp_block";
}
