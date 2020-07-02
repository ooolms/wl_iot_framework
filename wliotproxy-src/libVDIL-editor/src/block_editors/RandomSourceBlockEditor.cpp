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

#include "block_editors/RandomSourceBlockEditor.h"
#include "RandomSourceBlockEditorWidget.h"
#include "VDIL/blocks/RandomSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString RandomSourceBlockEditor::typeName()const
{
	return "random source";
}

QWidget* RandomSourceBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new RandomSourceBlockEditorWidget(parent);
}

void RandomSourceBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	RandomSourceBlockEditorWidget *w=(RandomSourceBlockEditorWidget*)editingWidget;
	const RandomSourceBlock *b=(const RandomSourceBlock*)block;
	w->setBounds(b->bounds());
}

void RandomSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	RandomSourceBlockEditorWidget *w=(RandomSourceBlockEditorWidget*)editingWidget;
	RandomSourceBlock *b=(RandomSourceBlock*)block;
	b->setBounds(w->bounds());
}

QPixmap RandomSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString RandomSourceBlockEditor::description()const
{
	return "generates multidimensional random value in specified bounds";
}

QString RandomSourceBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	RandomSourceBlock *b=(RandomSourceBlock*)block;
	const auto &bounds=b->bounds();
	QStringList strs;
	for(const auto &b:bounds)
		strs.append("("+QString::number(b.first)+":"+QString::number(b.second)+")");
	return "bounds: {"+strs.join(';')+"}";
}

QString RandomSourceBlockEditor::treeName()const
{
	return "Random source";
}
