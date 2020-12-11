/*******************************************
Copyright 2017 "Bespalov Denis"<master1312@yandex.ru>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ARPCTIMERONMILLIS_H
#define ARPCTIMERONMILLIS_H

class ARpcTimerOnMillis
{
public:
	typedef void (*Handler)(void*);

	class TimerEvent
	{
	public:
		TimerEvent();
		~TimerEvent();
		void single(unsigned long msec);//exec event in m milliseconds from current
		void repeat(unsigned long msec);//exec event each m milliseconds from current
		void reset();//reset time from now, not execution policy, disabled timer is still disabled
		void disable();

	public:
		enum StateFlags:unsigned char
		{
			Disabled,
			Single,
			Repeated
		};

	private:
		StateFlags state;
		Handler exec;
		unsigned long lastExecuteMillis;
		unsigned long timerDelta;
		void *data;
		TimerEvent *next;
		friend class ARpcTimerOnMillis;
	};

public:
	explicit ARpcTimerOnMillis();
	~ARpcTimerOnMillis();
	TimerEvent* addEvent(ARpcTimerOnMillis::Handler h,void *data);
	TimerEvent* eventAt(int index);
	void loop();//call this from loop() to check if there are events to be executed

private:
	unsigned char count;
	TimerEvent *firstEvent,*lastEvent;
};

#endif
