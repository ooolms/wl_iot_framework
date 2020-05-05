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

#include "block_editors/StorageSourceBlockEditor.h"
#include "StorageSourceBlockEditorWidget.h"
#include "GDIL/blocks/StorageSourceBlock.h"
#include "GDIL/core/Program.h"

QString StorageSourceBlockEditor::typeName() const
{
	return "storage source";
}

QWidget *StorageSourceBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new StorageSourceBlockEditorWidget(editor,parent);
}

void StorageSourceBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	StorageSourceBlockEditorWidget *w=(StorageSourceBlockEditorWidget*)editingWidget;
	const StorageSourceBlock *b=(const StorageSourceBlock*)block;
	QString devName;
	if(b->program()&&!b->storageId().deviceId.isNull())
		devName=b->program()->findDevName(b->storageId().deviceId);
	w->setParams(b->storageId(),devName,b->valuesType(),b->count(),b->needDevice());
}

void StorageSourceBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	StorageSourceBlockEditorWidget *w=(StorageSourceBlockEditorWidget*)editingWidget;
	StorageSourceBlock *b=(StorageSourceBlock*)block;
	b->setParams(w->storageId(),w->valuesType(),w->count(),w->needDevice());
}

QPixmap StorageSourceBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/storage_source.png");
}

QString StorageSourceBlockEditor::description()const
{
	return "storage source block";
}
