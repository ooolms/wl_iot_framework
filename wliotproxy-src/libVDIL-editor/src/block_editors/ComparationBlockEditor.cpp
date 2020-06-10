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
#include "VDIL/blocks/ComparationBlock.h"
#include "ComparationBlockEditorWidget.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString ComparationBlockEditor::typeName()const
{
	return "comparation";
}

QWidget* ComparationBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new ComparationBlockEditorWidget(parent);
}

void ComparationBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	ComparationBlockEditorWidget *w=(ComparationBlockEditorWidget*)editingWidget;
	const ComparationBlock *b=(const ComparationBlock*)block;
	w->setParams(b->outMode(),b->externalV2Input(),b->dimIndex(),b->operation());
	w->setDistValue(b->distValue());
	w->setV2Value(b->v2Value());
}

void ComparationBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	ComparationBlockEditorWidget *w=(ComparationBlockEditorWidget*)editingWidget;
	ComparationBlock *b=(ComparationBlock*)block;
	b->setParams(w->outMode(),w->externalV2Input(),w->dimIndex(),w->operation());
	b->setDistValue(w->distValue());
	b->setV2Value(w->v2Value());
}

QPixmap ComparationBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/comparation.png");
}

QString ComparationBlockEditor::description()const
{
	return "comparing 2 values";
}

QString ComparationBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	QString hint;
	ComparationBlock *b=(ComparationBlock*)block;
	QString v2Str;
	if(b->externalV2Input())
		v2Str="v2";
	else v2Str=QString::fromUtf8(b->v2Value().value()->valueToString(0));

	if(b->operation()==ComparationBlock::EQ)
		hint="v1=="+v2Str;
	else if(b->operation()==ComparationBlock::NEQ)
		hint="v1!="+v2Str;
	else if(b->operation()==ComparationBlock::GT)
		hint="v1>"+v2Str;
	else if(b->operation()==ComparationBlock::LT)
		hint="v1<"+v2Str;
	else if(b->operation()==ComparationBlock::GTEQ)
		hint="v1>="+v2Str;
	else if(b->operation()==ComparationBlock::LTEQ)
		hint="v1<="+v2Str;
	else hint="|v1-"+v2Str+"|<"+QString::fromUtf8(b->distValue().value()->valueToString(0));

	if(b->outMode()==ComparationBlock::SINGLE_BOOL)
		hint+=", boolean output";
	else if(b->outMode()==ComparationBlock::SPLITTED_BOOL)
		hint+=", splitted boolean output";
	else hint+=", splitted v1 output";
	return hint;
}
