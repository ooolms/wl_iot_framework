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

#include "block_editors/DateTimeSourceBlockEditor.h"
#include "DateTimeSourceBlockEditorWidget.h"
#include "GDIL/blocks/DateTimeSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

QWidget* DateTimeSourceBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new DateTimeSourceBlockEditorWidget(parent);
}

void DateTimeSourceBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	DateTimeSourceBlockEditorWidget *w=(DateTimeSourceBlockEditorWidget*)editingWidget;
	const DateTimeSourceBlock *b=(const DateTimeSourceBlock*)block;
	w->setDateOutputs(b->dateOutputs());
}

void DateTimeSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	DateTimeSourceBlockEditorWidget *w=(DateTimeSourceBlockEditorWidget*)editingWidget;
	DateTimeSourceBlock *b=(DateTimeSourceBlock*)block;
	b->setDateOutputs(w->dateOutputs());
}

QPixmap DateTimeSourceBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/date_source.png");
}

QString DateTimeSourceBlockEditor::description()const
{
	return "outputs current date+time source";
}

QString DateTimeSourceBlockEditor::typeName()const
{
	return "date+time block";
}

QString DateTimeSourceBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	return "date+time";
}
