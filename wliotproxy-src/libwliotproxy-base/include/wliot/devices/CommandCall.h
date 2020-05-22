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

#ifndef COMMANDCALL_H
#define COMMANDCALL_H

#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/devices/Message.h"
#include <QObject>
#include <QTimer>

namespace WLIOT
{

	class RealDevice;

	class CommandCall
		:public QObject
	{
		Q_OBJECT
	public:
		explicit CommandCall(const QByteArray &cmd,QObject *parent=0);
		const QByteArrayList& returnValue();
		CommandCall* setArgs(const QByteArrayList &args);
		CommandCall* setUseCallMsg(bool u);
		CommandCall* setupTimer(int msec);
		CommandCall* setRecallOnDevReset(bool en);
		bool ok();
		void abort();
		bool wait();
		bool isWorking();
		QByteArray callId();

	signals:
		void done();

	private slots:
		void onDeviceDisconnected();
		void onDeviceDestroyed();
		void onDeviceReset();
		void onTimeout();

	private:
		void run(RealDevice *d,const QByteArray &callIdStr);
		void onOkMsg(const QByteArrayList &args);
		void onErrorMsg(const QByteArray &msg);
		void onErrorMsg(const QByteArrayList &args);

	private:
		friend class RealDevice;
		QByteArrayList retVal;
		bool mOk;
		enum
		{
			WAIT,
			EXEC,
			DONE
		}state;
		RealDevice *dev;
		QTimer timer;
		QByteArray mCommand;
		QByteArrayList mArgs;
		bool mUseCallMsg,mRecallOnDevReset;
		QByteArray mCallId;
	};
}

#endif // COMMANDCALL_H
