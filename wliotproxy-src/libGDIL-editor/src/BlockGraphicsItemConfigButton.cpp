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

#include "BlockGraphicsItemConfigButton.h"
#include "BlockGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>

using namespace WLIOT;
using namespace WLIOTGDIL;

BlockGraphicsItemConfigButton::BlockGraphicsItemConfigButton(const QPixmap &pm,BlockGraphicsItem *block)
	:QGraphicsPixmapItem(pm,block)
{
	mBlock=block;
}

void BlockGraphicsItemConfigButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	if(event->button()==Qt::LeftButton)
	{
		event->accept();
		mBlock->onSettingsClicked();
	}
}
