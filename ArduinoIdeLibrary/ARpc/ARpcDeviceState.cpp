/*******************************************
Copyright 2018 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcDeviceState.h"
#include "ARpcRealDeviceMessageDispatch.h"
#include "ARpcArduStrHlp.h"
#include <stdlib.h>

ARpcDeviceState::ARpcDeviceState(ARpcStreamWriter *wr)
{
	mCommands=0;
	mAddParams=0;
	mCommandsCount=mAddParamsCount=0;
	writer=wr;
}

ARpcDeviceState::~ARpcDeviceState()
{
	if(mCommands)
		delete[] mCommands;
	if(mAddParams)
		delete[] mAddParams;
}

void ARpcDeviceState::prepareCommands(unsigned int count)
{
	if(mCommands)return;
	mCommandsCount=count;
	mCommands=new CommandState[count];
}

void ARpcDeviceState::prepareCommand(unsigned int index,const char *commandName,unsigned int paramsCount)
{
	if(index>=mCommandsCount)return;
	mCommands[index].prepare(commandName,paramsCount);
}

void ARpcDeviceState::prepareAdditionalParameters(unsigned int count)
{
	if(mAddParams)return;
	mAddParamsCount=count;
	mAddParams=new AdditionalParamState[count];
}

void ARpcDeviceState::prepareAdditionalParameter(unsigned int index,const char *name)
{
	if(index>=mAddParamsCount)return;
	mAddParams[index].prepare(name);
}

void ARpcDeviceState::setCommandParamState(unsigned int commandIndex,unsigned int paramIndex,const char *value)
{
	if(commandIndex>=mCommandsCount)return;
	if(paramIndex>=mCommands[commandIndex].paramsCount)return;
	mCommands[commandIndex].updateValue(paramIndex,value);
	notifyCommandParamChanged(commandIndex,paramIndex);
}

void ARpcDeviceState::setAdditionalParamState(unsigned int index,const char *value)
{
	if(index>=mAddParamsCount)return;
	mAddParams[index].updateValue(value);
	notifyAdditionalParamChanged(index);
}

void ARpcDeviceState::dump()
{
	for(unsigned char i=0;i<mCommandsCount;++i)
		for(unsigned char j=0;j<mCommands[i].paramsCount;++j)
			writeCommandParamState(i,j);
	for(unsigned char i=0;i<mAddParamsCount;++i)
		writeAdditionalParamState(i);
}

void ARpcDeviceState::notifyCommandParamChanged(unsigned int commandIndex, unsigned int paramIndex)
{
	if(!writer->beginWriteMsg())return;
	writer->writeArgNoEscape(F("statechanged"));
	writeCommandParamState(commandIndex,paramIndex);
	writer->endWriteMsg();
}

void ARpcDeviceState::writeCommandParamState(unsigned int commandIndex,unsigned int paramIndex)
{
	writer->writeArg(mCommands[commandIndex].command,strlen(mCommands[commandIndex].command));
	writeUInt(paramIndex+1);
	char *v=mCommands[commandIndex].paramsValues[paramIndex];
	if(v)writer->writeArg(v,strlen(v));
	else writer->writeArgNoEscape(F(""));
}

void ARpcDeviceState::notifyAdditionalParamChanged(unsigned int index)
{
	if(!writer->beginWriteMsg())return;
	writer->writeArgNoEscape(F("statechanged"));
	writeAdditionalParamState(index);
	writer->endWriteMsg();
}

void ARpcDeviceState::writeAdditionalParamState(unsigned int index)
{
	writer->writeArgNoEscape(F("#"));
	char *v=mAddParams[index].paramValue;
	writer->writeArg(mAddParams[index].paramName,strlen(mAddParams[index].paramName));
	if(v)writer->writeArg(v,strlen(v));
	else writer->writeArgNoEscape(F(""));
}

void ARpcDeviceState::writeUInt(unsigned int c)
{
	char str[11];
	str[10]=0;
	unsigned char i=0;
	do
	{
		char d=c%10;
		str[9-i]=d+'0';
		c/=10;
		++i;
	}
	while(c!=0);
	writer->writeArgNoEscape(str+10-i);
}

ARpcDeviceState::CommandState::CommandState()
{
	command=0;
	paramsCount=0;
	paramsValues=0;
}

ARpcDeviceState::CommandState::~CommandState()
{
	if(command)delete[] command;
	if(paramsValues)
	{
		for(int i=0;i<paramsCount;++i)
			if(paramsValues[i])
				delete[] paramsValues[i];
		delete[] paramsValues;
	}
}

void ARpcDeviceState::CommandState::prepare(const char *cmd,unsigned int pCount)
{
	if(command)return;
	command=new char[strlen(cmd)+1];
	strcpy(command,cmd);
	paramsCount=pCount;
	if(paramsCount==0)
		paramsValues=0;
	else
	{
		paramsValues=new char*[pCount];
		memset(paramsValues,0,paramsCount*sizeof(char*));
	}
}

void ARpcDeviceState::CommandState::updateValue(unsigned int index,const char *val)
{
	if(index>=paramsCount)return;
	if(paramsValues[index])
		delete paramsValues[index];
	paramsValues[index]=new char[strlen(val)+1];
	strcpy(paramsValues[index],val);
}

ARpcDeviceState::AdditionalParamState::AdditionalParamState()
{
	paramName=paramValue=0;
}

ARpcDeviceState::AdditionalParamState::~AdditionalParamState()
{
	if(paramName)delete[] paramName;
	if(paramValue)delete[] paramValue;
}

void ARpcDeviceState::AdditionalParamState::prepare(const char *name)
{
	if(paramName)return;
	paramName=new char[strlen(name)+1];
	strcpy(paramName,name);
}

void ARpcDeviceState::AdditionalParamState::updateValue(const char *val)
{
	if(paramValue)
		delete[] paramValue;
	paramValue=new char[strlen(val)+1];
	strcpy(paramValue,val);
}
