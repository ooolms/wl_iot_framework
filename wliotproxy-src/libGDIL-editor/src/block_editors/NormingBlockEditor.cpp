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

#include "block_editors/NormingBlockEditor.h"
#include "NormingBlockEditorWidget.h"
#include "GDIL/blocks/NormingBlock.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

QString NormingBlockEditor::typeName()const
{
	return "linear transformation";
}

QWidget* NormingBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new NormingBlockEditorWidget(parent);
}

void NormingBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	NormingBlockEditorWidget *w=(NormingBlockEditorWidget*)editingWidget;
	const NormingBlock *b=(const NormingBlock*)block;
	w->setParams(b->minX(),b->maxX(),b->minY(),b->maxY(),b->dimIndex(),b->forceLimits());
}

void NormingBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	NormingBlockEditorWidget *w=(NormingBlockEditorWidget*)editingWidget;
	NormingBlock *b=(NormingBlock*)block;
	DataUnit minX,maxX,minY,maxY;
	w->limits(minX,maxX,minY,maxY);
	quint32 dimIndex=w->dimIndex();
	bool forceLimits=w->forceLimits();
	if(minX.numType()==DataUnit::S64)
		b->setParams(minX.value()->valueToS64(0),maxX.value()->valueToS64(0),minY.value()->valueToS64(0),
			maxY.value()->valueToS64(0),dimIndex,forceLimits);
	else b->setParams(minX.value()->valueToDouble(0),maxX.value()->valueToDouble(0),minY.value()->valueToDouble(0),
		maxY.value()->valueToDouble(0),dimIndex,forceLimits);
}

QPixmap NormingBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/norming.png");
}

QString NormingBlockEditor::description()const
{
	return "linear transformation of input value (normalization)";
}

QString NormingBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	QString hint;
	NormingBlock *b=(NormingBlock*)block;
	if(b->minX().numType()==DataUnit::S64)
		hint=QString::fromUtf8("("+QByteArray::number(b->minX().value()->valueToS64(0))+","+
			QByteArray::number(b->maxX().value()->valueToS64(0))+")->("+
			QByteArray::number(b->minY().value()->valueToS64(0))+","+
			QByteArray::number(b->maxY().value()->valueToS64(0))+")");
	else hint=QString::fromUtf8("("+QByteArray::number(b->minX().value()->valueToDouble(0))+","+
		QByteArray::number(b->maxX().value()->valueToDouble(0))+")->("+
		QByteArray::number(b->minY().value()->valueToDouble(0))+","+
		QByteArray::number(b->maxY().value()->valueToDouble(0))+")");
	if(b->forceLimits())
		hint+="; output limited to bounds";
	return hint;
}
