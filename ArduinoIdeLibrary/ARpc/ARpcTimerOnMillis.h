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

#ifndef ARDUINO
#error "Only for Arduino IDE, remove this class from project
#endif

class ARpcTimerOnMillis
{
public:
	typedef void (*Handler)();

private:
	class TimerEvent
	{
	public:
		TimerEvent();

	public:
		static const char ENABLE_BIT=0x1;
		static const char REPEATED_BIT=0x2;

	public:
		unsigned char state;
		Handler exec;
		unsigned long nextExecuteMillis;
		unsigned long timerDelta;
	};

public:
	explicit ARpcTimerOnMillis(unsigned char c);
	~ARpcTimerOnMillis();
	void setEventHandler(unsigned char index,ARpcTimerOnMillis::Handler h);
	void execInMillis(unsigned char index,unsigned long m);//exec event in m milliseconds from current
	void execRepeated(unsigned char index,unsigned long m);//exec event each m milliseconds from current
	void resetTimer(unsigned char index);
	void disable(unsigned char index);
	void process();//call this from loop() to check if there are events to be executed

private:
	unsigned char count;
	TimerEvent *events;
};

#endif
