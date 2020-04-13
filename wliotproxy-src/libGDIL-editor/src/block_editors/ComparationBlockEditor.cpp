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

#include "block_editors/ComparationBlockEditor.h"
#include "GDIL/blocks/ComparationBlock.h"
#include "ComparationBlockEditorWidget.h"

QString ComparationBlockEditor::typeName()const
{
	return "compare values";
}

QWidget* ComparationBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	Q_UNUSED(editor)
	return new ComparationBlockEditorWidget(parent);
}

void ComparationBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	ComparationBlockEditorWidget *w=(ComparationBlockEditorWidget*)editingWidget;
	const ComparationBlock *b=(const ComparationBlock*)block;
	w->setParams(b->outMode(),b->externalV2Input(),b->dimIndex(),b->operation());
	w->setDistValue(b->distValue());
	w->setV2Value(b->v2Value());
}

void ComparationBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	ComparationBlockEditorWidget *w=(ComparationBlockEditorWidget*)editingWidget;
	ComparationBlock *b=(ComparationBlock*)block;
	b->setParams(w->outMode(),w->externalV2Input(),w->dimIndex(),w->operation());
	b->setDistValue(w->distValue());
	b->setV2Value(w->v2Value());
}

QPixmap ComparationBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/comparation.png");
}

QString ComparationBlockEditor::description()const
{
	return "comparing 2 values";
}
