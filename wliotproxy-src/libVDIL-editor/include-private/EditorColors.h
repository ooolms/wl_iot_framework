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

#ifndef EDITORCOLORS_H
#define EDITORCOLORS_H

#include <QColor>

namespace WLIOTVDIL
{
	class EditorColors
	{
	public:
		static const QColor boolTypeColor;
		static const QColor singleTypeColor;
		static const QColor arrayTypeColor;
		static const QColor dateTimeTypeColor;
		static const QColor anyTypeColor;
	};
}

#endif // EDITORCOLORS_H
