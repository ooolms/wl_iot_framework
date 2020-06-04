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

#ifndef DATETIMESOURCEBLOCKEDITOR_H
#define DATETIMESOURCEBLOCKEDITOR_H

#include "GDIL/editor/IBlockEditor.h"

namespace WLIOTGDIL
{
	class DateTimeSourceBlockEditor
		:public IBlockEditor
	{
	public:
		virtual QWidget *mkEditingWidget(IEditorHelper *helper,QWidget *parent)override;
		virtual void loadParamsFromBlock(IEditorHelper *helper,QWidget *editingWidget,const BaseBlock *block)override;
		virtual void saveParamsToBlock(IEditorHelper *helper,QWidget *editingWidget,BaseBlock *block)override;
		virtual QPixmap previewImage()const override;
		virtual QString description()const override;
		virtual QString typeName()const override;
		virtual QString hint(IEditorHelper *helper,BaseBlock *block)const override;
	};
}

#endif // DATETIMESOURCEBLOCKEDITOR_H
