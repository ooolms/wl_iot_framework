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

#include "ARpcTimerOnMillis.h"
#include <Arduino.h>

ARpcTimerOnMillis::TimerEvent::TimerEvent()
{
	state=0;
	exec=0;
	timerDelta=nextExecuteMillis=0;
}

ARpcTimerOnMillis::ARpcTimerOnMillis(unsigned char c)
{
	count=c;
	events=new TimerEvent[count];
}

ARpcTimerOnMillis::~ARpcTimerOnMillis()
{
	delete events;
}

void ARpcTimerOnMillis::setEventHandler(unsigned char index,ARpcTimerOnMillis::Handler h)
{
	if(index>=count)return;
	events[index].exec=h;
}

void ARpcTimerOnMillis::execInMillis(unsigned char index,unsigned long m)
{
	if(index>=count)return;
	TimerEvent &e=events[index];
	e.timerDelta=m;
	e.nextExecuteMillis=millis()+m;
	e.state=TimerEvent::ENABLE_BIT;
}

void ARpcTimerOnMillis::execRepeated(unsigned char index,unsigned long m)
{
	if(index>=count)return;
	TimerEvent &e=events[index];
	e.timerDelta=m;
	e.nextExecuteMillis=millis()+m;
	e.state=TimerEvent::ENABLE_BIT|TimerEvent::REPEATED_BIT;
}

void ARpcTimerOnMillis::resetTimer(unsigned char index)
{
	if(index>=count)return;
	TimerEvent &e=events[index];
	if(e.timerDelta==0)return;
	e.nextExecuteMillis=millis()+e.timerDelta;
	e.state|=TimerEvent::ENABLE_BIT;
}

void ARpcTimerOnMillis::disable(unsigned char index)
{
	if(index>=count)return;
	events[index].state&=~TimerEvent::ENABLE_BIT;
}

void ARpcTimerOnMillis::process()
{
	unsigned long m=millis();
	for(unsigned char i=0;i<count;++i)
	{
		if(!(events[i].state&TimerEvent::ENABLE_BIT)||events[i].exec==0)continue;
		if(m>=events[i].nextExecuteMillis)
		{
			events[i].exec();
			events[i].nextExecuteMillis+=events[i].timerDelta;
			if(!(events[i].state&TimerEvent::REPEATED_BIT))
				events[i].state&=~TimerEvent::ENABLE_BIT;
		}
	}
}
