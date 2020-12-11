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
	state=Disabled;
	exec=0;
	data=0;
	next=0;
	timerDelta=lastExecuteMillis=0;
}

ARpcTimerOnMillis::TimerEvent::~TimerEvent()
{
	if(next)
		delete next;
}

ARpcTimerOnMillis::ARpcTimerOnMillis()
{
	count=0;
	firstEvent=lastEvent=0;
}

ARpcTimerOnMillis::~ARpcTimerOnMillis()
{
	if(firstEvent)
		delete firstEvent;
}

ARpcTimerOnMillis::TimerEvent* ARpcTimerOnMillis::addEvent(ARpcTimerOnMillis::Handler h,void *data)
{
	TimerEvent *e=new TimerEvent;
	e->exec=h;
	e->data=data;
	if(!firstEvent)
		firstEvent=lastEvent=e;
	else
	{
		lastEvent->next=e;
		lastEvent=e;
	}
	return e;
}

ARpcTimerOnMillis::TimerEvent *ARpcTimerOnMillis::eventAt(int index)
{
	TimerEvent *e=firstEvent;
	for(int i=0;i<index;++i)
	{
		if(!e)return 0;
		e=e->next;
	}
	return e;
}

void ARpcTimerOnMillis::TimerEvent::single(unsigned long msec)
{
	timerDelta=msec;
	lastExecuteMillis=millis();
	state=TimerEvent::Single;
}

void ARpcTimerOnMillis::TimerEvent::repeat(unsigned long msec)
{
	timerDelta=msec;
	lastExecuteMillis=millis();
	state=TimerEvent::Repeated;
}

void ARpcTimerOnMillis::TimerEvent::reset()
{
	if(timerDelta==0||state==Disabled)return;
	lastExecuteMillis=millis();
}

void ARpcTimerOnMillis::TimerEvent::disable()
{
	state=Disabled;
}

void ARpcTimerOnMillis::loop()
{
	unsigned long m=millis();
	TimerEvent *e=firstEvent;
	while(e)
	{
		if(e->state!=TimerEvent::Disabled&&e->exec&&((m-e->lastExecuteMillis)>e->timerDelta))
		{
			e->exec(e->data);
			e->lastExecuteMillis=m;
			if(e->state==TimerEvent::Single)
				e->state=TimerEvent::Disabled;
		}
		e=e->next;
	}
}
