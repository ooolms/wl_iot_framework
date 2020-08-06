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

#include "LogicalBlocksGroupEditorsFactory.h"
#include "VDIL/blocks/LogicalBlocks.h"
#include "block_editors/DefaultBlockEditor.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

static const QString link="https://dev.alterozoom.com/doc/doku.php?id=iot:vdil:основные_блоки";

LogicalBlocksGroupEditorsFactory::LogicalBlocksGroupEditorsFactory()
{
	addEditor(NotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_not.png"),"logical NOT","NOT","NOT","logical NOT",link));
	addEditor(AndBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_and.png"),"logical AND","AND","AND","logical AND",link));
	addEditor(OrBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_or.png"),"logical OR","OR","OR","logical OR",link));
	addEditor(XorBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_xor.png"),"logical XOR","XOR","XOR","logical XOR",link));
	addEditor(AndNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_and_not.png"),
		"logical AND-NOT","AND-NOT","AND-NOT","logical AND-NOT",link));
	addEditor(OrNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_or_not.png"),"logical OR-NOT","OR-NOT","OR-NOT","logical OR-NOT",link));
	addEditor(XorNotBoolBlock::mBlockName,new DefaultBlockEditor(
		QPixmap(":/VDIL/editor/blocks/logic_xor_not.png"),
		"logical XOR-NOT","XOR-NOT","XOR-NOT","logical XOR-NOT",link));
}

QString LogicalBlocksGroupEditorsFactory::groupDisplayTitle()const
{
	return "Logical blocks";
}
