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

#ifndef BASEBLOCKSGROUPEDITORSFACTORY_H
#define BASEBLOCKSGROUPEDITORSFACTORY_H

#include "VDIL/editor/IBlocksGroupEditorsFactory.h"

namespace WLIOTVDIL
{
	class BaseBlocksGroupEditorsFactory
		:public IBlocksGroupEditorsFactory
	{
	public:
		~BaseBlocksGroupEditorsFactory();
		virtual QStringList allBlocks()const override;
		virtual IBlockEditor *editor(const QString &blockName)override;

	protected:
		void addEditor(const QString &blockName,IBlockEditor *editor);

	private:
		QStringList mBlockNames;
		QMap<QString,IBlockEditor*> mEditors;
	};
}

#endif // BASEBLOCKSGROUPEDITORSFACTORY_H
