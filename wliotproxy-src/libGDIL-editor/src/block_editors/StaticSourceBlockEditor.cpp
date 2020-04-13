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

#include "block_editors/StaticSourceBlockEditor.h"
#include "StaticSourceBlockEditorWidget.h"
#include "GDIL/blocks/StaticSourceBlock.h"

QString StaticSourceBlockEditor::typeName()const
{
	return "static source";
}

QWidget *StaticSourceBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new StaticSourceBlockEditorWidget(parent);
}

void StaticSourceBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	StaticSourceBlockEditorWidget *w=(StaticSourceBlockEditorWidget*)editingWidget;
	const StaticSourceBlock *b=(const StaticSourceBlock*)block;
	w->setValue(b->value());
}

void StaticSourceBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	StaticSourceBlockEditorWidget *w=(StaticSourceBlockEditorWidget*)editingWidget;
	StaticSourceBlock *b=(StaticSourceBlock*)block;
	b->setValue(w->value());
}

QPixmap StaticSourceBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/static_source.png");
}

QString StaticSourceBlockEditor::description()const
{
	return "static source block";
}
