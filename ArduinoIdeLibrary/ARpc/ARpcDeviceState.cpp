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
#include "ARpcConfig.h"
#include <stdlib.h>
#include <string.h>

ARpcDeviceState::ARpcDeviceState(ARpcRealDeviceMessageDispatch *p)
{
	mCommands=0;
	mAddParams=0;
	mCommandsCount=mAddParamsCount=0;
	disp=p;
}

ARpcDeviceState::~ARpcDeviceState()
{
	if(mCommands)
		delete[] mCommands;
	if(mAddParams)
		delete[] mAddParams;
}

void ARpcDeviceState::prepareCommands(unsigned char count)
{
	if(mCommands)return;
	mCommandsCount=count;
	mCommands=new CommandState[count];
}

void ARpcDeviceState::prepareCommand(unsigned char index,const char *commandName,unsigned char paramsCount)
{
	if(index>=mCommandsCount)return;
	mCommands[index].prepare(commandName,paramsCount);
}

void ARpcDeviceState::prepareAdditionalParameters(unsigned char count)
{
	if(mAddParams)return;
	mAddParamsCount=count;
	mAddParams=new AdditionalParamState[count];
}

void ARpcDeviceState::prepareAdditionalParameter(unsigned char index,const char *name)
{
	if(index>=mAddParamsCount)return;
	mAddParams[index].prepare(name);
}

void ARpcDeviceState::setCommandParamState(unsigned char commandIndex,unsigned char paramIndex,const char *value)
{
	if(commandIndex>=mCommandsCount)return;
	if(paramIndex>=mCommands[commandIndex].paramsCount)return;
	mCommands[commandIndex].updateValue(paramIndex,value);
	notifyCommandParamChanged(commandIndex,paramIndex);
}

void ARpcDeviceState::setAdditionalParamState(unsigned char index,const char *value)
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

void ARpcDeviceState::notifyCommandParamChanged(unsigned char commandIndex,unsigned char paramIndex)
{
	if(!disp->writer()->beginWriteMsg())return;
	disp->writer()->writeArgNoEscape(stateChangedMsg);
	writeCommandParamState(commandIndex,paramIndex);
	disp->writer()->endWriteMsg();
}

void ARpcDeviceState::writeCommandParamState(unsigned char commandIndex,unsigned char paramIndex)
{
	disp->writer()->writeArg(mCommands[commandIndex].command,strlen(mCommands[commandIndex].command));
	writeUChar(paramIndex+1);
	char *v=mCommands[commandIndex].paramsValues[paramIndex];
	if(v)disp->writer()->writeArg(v,strlen(v));
	else disp->writer()->writeArgNoEscape("");
}

void ARpcDeviceState::notifyAdditionalParamChanged(unsigned char index)
{
	if(!disp->writer()->beginWriteMsg())return;
	disp->writer()->writeArgNoEscape(stateChangedMsg);
	writeAdditionalParamState(index);
	disp->writer()->endWriteMsg();
}

void ARpcDeviceState::writeAdditionalParamState(unsigned char index)
{
	disp->writer()->writeArgNoEscape("#");
	char *v=mAddParams[index].paramValue;
	disp->writer()->writeArg(mAddParams[index].paramName,strlen(mAddParams[index].paramName));
	if(v)disp->writer()->writeArg(v,strlen(v));
	else disp->writer()->writeArgNoEscape("");
}

void ARpcDeviceState::writeUChar(unsigned char c)
{
	unsigned char t=c/100;
	char str[4];
	unsigned char i=0;
	if(t!=0)
	{
		*str=(t+'0');
		++i;
	}
	t=((c%100)/10);
	if(t!=0)
	{
		str[i]=(t+'0');
		++i;
	}
	str[i]=(c%10)+'0';
	str[++i]=0;
	disp->writer()->writeArgNoEscape(str);
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

void ARpcDeviceState::CommandState::prepare(const char *cmd,unsigned char pCount)
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

void ARpcDeviceState::CommandState::updateValue(unsigned char index,const char *val)
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
