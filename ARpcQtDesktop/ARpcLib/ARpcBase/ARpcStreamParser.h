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

#ifndef ARPCSTREAMPARSER_H
#define ARPCSTREAMPARSER_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcIMessageHandler.h"
#include <QObject>
#include <functional>

class ARpcStreamParser
	:public QObject
{
	Q_OBJECT

public:
	typedef std::function<void(const ARpcMessage&)> MessageHandler;

public:
	explicit ARpcStreamParser(QObject *parent=0);
	void pushData(const QByteArray &data);
	void reset();
	static QByteArray dump(const ARpcMessage &m);//adds msgDelim

signals:
	void processMessage(const ARpcMessage &m);

private:
	inline void parseCharInNormalState(char c);
	inline void parseCharInEscapeState(char c);

private:
	enum
	{
		NORMAL,
		ESCAPE,//next symbol is escaped
		ESCAPE_HEX1,//next symbol is first of 2-letter hex code
		ESCAPE_HEX2//next symbol is second of 2-letter hex code
	}state;
	ARpcMessage newMessage;
	QByteArray *currentFilledStr;
	QByteArray hexChars;
};

#endif // ARPCSTREAMPARSER_H
