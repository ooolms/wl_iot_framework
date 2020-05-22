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

#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include "wliot/devices/Message.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/devices/IMessageHandler.h"
#include <QObject>

namespace WLIOT
{
	class StreamParser
		:public QObject
	{
		Q_OBJECT
	public:
		explicit StreamParser(QObject *parent=0);
		void pushData(const QByteArray &data);
		void reset();
		static bool tryParse(const QByteArray &data,Message &m);

	signals:
		void newMessage(const Message &m);
		void streamWasReset();

	private:
		inline void parseCharInNormalState(char c);
		inline void parseCharInEscapeState(char c);

	private:
		enum
		{
			NORMAL,
			ESCAPE,//next symbol is escaped
			ESCAPE_HEX1,//next symbol is first of 2-letter hex code
			ESCAPE_HEX2,//next symbol is second of 2-letter hex code
			NEXT_MESSAGE//message just parsed, next char will be in a next message
		}state;
		Message nextMessage;
		QByteArray *currentFilledStr;
		QByteArray hexChars;
	};
}

#endif // STREAMPARSER_H
