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

#include "block_editors/SubProgramBlockEditor.h"
#include "SubProgramBlockEditorWidget.h"
#include "VDIL/core/SubProgramBlock.h"
#include "VDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

QString SubProgramBlockEditor::typeName() const
{
	return "subprogram";
}

QWidget *SubProgramBlockEditor::mkEditingWidget(IEditorHelper *,QWidget *parent)
{
	return new SubProgramBlockEditorWidget(parent);
}

void SubProgramBlockEditor::loadParamsFromBlock(IEditorHelper *,QWidget *editingWidget,const BaseBlock *block)
{
	SubProgramBlockEditorWidget *w=(SubProgramBlockEditorWidget*)editingWidget;
	const SubProgramBlock *b=(const SubProgramBlock*)block;
	w->setParams(b->inputTypes(),b->inputTitles(),b->outputTypes(),b->outputTitles());
}

void SubProgramBlockEditor::saveParamsToBlock(IEditorHelper *,QWidget *editingWidget,BaseBlock *block)
{
	SubProgramBlockEditorWidget *w=(SubProgramBlockEditorWidget*)editingWidget;
	SubProgramBlock *b=(SubProgramBlock*)block;
	b->setParams(w->inputsTypes(),w->inputsTitles(),w->outputsTypes(),w->outputsTitles());
}

QPixmap SubProgramBlockEditor::previewImage()const
{
	return QPixmap(":/VDIL/editor/blocks/empty.png");
}

QString SubProgramBlockEditor::description()const
{
	return "VDIL subprogram";
}

QString SubProgramBlockEditor::hint(IEditorHelper *,BaseBlock *)const
{
	return QString();
}

QString SubProgramBlockEditor::treeName()const
{
	return "VDIL subprogram";
}
