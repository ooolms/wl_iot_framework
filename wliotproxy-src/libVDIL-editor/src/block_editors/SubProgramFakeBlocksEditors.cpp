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


#include "block_editors/SubProgramFakeBlocksEditors.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString SubProgramInternalOutputsFakeBlockEditor::typeName()const
{
	return "start";
}

QWidget* SubProgramInternalOutputsFakeBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *)
{
	return 0;
}

void SubProgramInternalOutputsFakeBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *,const BaseBlock *)
{
}

void SubProgramInternalOutputsFakeBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *,BaseBlock *)
{
}

QPixmap SubProgramInternalOutputsFakeBlockEditor::previewImage()const
{
	return QPixmap();
}

QString SubProgramInternalOutputsFakeBlockEditor::description()const
{
	return "";
}

QString SubProgramInternalOutputsFakeBlockEditor::hint(IEditorHelper *,BaseBlock *)const
{
	return QString("start");
}

QString SubProgramInternalOutputsFakeBlockEditor::treeName()const
{
	return "";
}

QString SubProgramInternalOutputsFakeBlockEditor::wikiLink()const
{
	return QString();
}

QString SubProgramInternalInputsFakeBlockEditor::typeName()const
{
	return "end";
}

QWidget* SubProgramInternalInputsFakeBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *)
{
	return 0;
}

void SubProgramInternalInputsFakeBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *,const BaseBlock *)
{
}

void SubProgramInternalInputsFakeBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *,BaseBlock *)
{
}

QPixmap SubProgramInternalInputsFakeBlockEditor::previewImage()const
{
	return QPixmap();
}

QString SubProgramInternalInputsFakeBlockEditor::description()const
{
	return "";
}

QString SubProgramInternalInputsFakeBlockEditor::hint(IEditorHelper *,BaseBlock *)const
{
	return QString("end");
}

QString SubProgramInternalInputsFakeBlockEditor::treeName()const
{
	return "";
}

QString SubProgramInternalInputsFakeBlockEditor::wikiLink()const
{
	return QString();
}
