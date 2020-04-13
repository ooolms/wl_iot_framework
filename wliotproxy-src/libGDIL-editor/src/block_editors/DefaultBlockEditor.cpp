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

#include "block_editors/DefaultBlockEditor.h"

DefaultBlockEditor::DefaultBlockEditor(const QPixmap &preview, const QString &descr, const QString &typeName)
{
	mPreview=preview;
	mDescription=descr;
	mTypeName=typeName;
}

QString DefaultBlockEditor::typeName() const
{
	return mTypeName;
}

QWidget* DefaultBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *)
{
	return 0;
}

void DefaultBlockEditor::loadParamsFromBlock(QWidget *,const BaseBlock *)
{
}

void DefaultBlockEditor::saveParamsToBlock(QWidget *,BaseBlock *)
{
}

QPixmap DefaultBlockEditor::previewImage()const
{
	return mPreview;
}

QString DefaultBlockEditor::description()const
{
	return mDescription;
}
