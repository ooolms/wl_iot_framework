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
#include "VDIL/blocks/StaticSourceBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString StaticSourceBlockEditor::typeName()const
{
	return "static source";
}

QWidget* StaticSourceBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new StaticSourceBlockEditorWidget(parent);
}

void StaticSourceBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	StaticSourceBlockEditorWidget *w=(StaticSourceBlockEditorWidget*)editingWidget;
	const StaticSourceBlock *b=(const StaticSourceBlock*)block;
	w->setParams(b->value(),b->configurable());
}

void StaticSourceBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	StaticSourceBlockEditorWidget *w=(StaticSourceBlockEditorWidget*)editingWidget;
	StaticSourceBlock *b=(StaticSourceBlock*)block;
	b->setParams(w->value(),w->configurable());
}

QPixmap StaticSourceBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/static_source.png");
}

QString StaticSourceBlockEditor::description()const
{
	return "static source block";
}

QString StaticSourceBlockEditor::hint(IEditorHelper *,BaseBlock *block)const
{
	QString hint;
	StaticSourceBlock *b=(StaticSourceBlock*)block;
	if(!b->value().isValid())
		hint.clear();
	else if(b->value().type()==DataUnit::BOOL)
		hint=QString("bool: ")+QString::fromUtf8(b->value().boolValue()?"true":"false");
	else if(b->value().type()==DataUnit::ARRAY)
		hint=QString("array: ")+QString::fromUtf8(b->value().value()->dumpToMsgArgs().join("|"));
	else if(b->value().type()==DataUnit::DATETIME)
		hint=QString("date+time: ")+b->value().dateTimeValue().toString(Qt::ISODate);
	else hint=QString("single: ")+QString::fromUtf8(b->value().value()->valueToString(0));

	if(b->configurable())
		hint+=", configurable";
	return hint;
}
