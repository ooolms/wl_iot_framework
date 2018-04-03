#include "ARpcDeviceState.h"
#include "ARpc.h"
#include <stdlib.h>
#include <string.h>

static const char *stateChangedMsg="statechanged";

ARpcDeviceState::ARpcDeviceState(ARpc *p)
{
	mCommands=0;
	mAddParams=0;
	mCommandsCount=mAddParamsCount=0;
	parser=p;
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
			if(mCommands[i].paramsValues[j])
				writeCommandParamState(i,j);
	for(unsigned char i=0;i<mAddParamsCount;++i)
		if(mAddParams[i].paramValue)
			writeAdditionalParamState(i);
}

void ARpcDeviceState::notifyCommandParamChanged(unsigned char commandIndex,unsigned char paramIndex)
{
	parser->writeCallback(stateChangedMsg,strlen(stateChangedMsg));
	parser->writeCallback("|",1);
	writeCommandParamState(commandIndex,paramIndex);
	parser->writeCallback("\n",1);
}

void ARpcDeviceState::writeCommandParamState(unsigned char commandIndex,unsigned char paramIndex)
{
	parser->writeData(mCommands[commandIndex].command,strlen(mCommands[commandIndex].command));
	parser->writeCallback("|",1);
	writeUChar(paramIndex+1);
	parser->writeCallback("|",1);
	parser->writeData(mCommands[commandIndex].paramsValues[paramIndex],
		strlen(mCommands[commandIndex].paramsValues[paramIndex]));
}

void ARpcDeviceState::notifyAdditionalParamChanged(unsigned char index)
{
	parser->writeCallback(stateChangedMsg,strlen(stateChangedMsg));
	parser->writeCallback("|",1);
	writeAdditionalParamState(index);
	parser->writeCallback("\n",1);
}

void ARpcDeviceState::writeAdditionalParamState(unsigned char index)
{
	parser->writeCallback("#|",2);
	parser->writeData(mAddParams[index].paramName,strlen(mAddParams[index].paramName));
	parser->writeCallback("|",1);
	parser->writeData(mAddParams[index].paramValue,strlen(mAddParams[index].paramValue));
}

void ARpcDeviceState::writeUChar(unsigned char c)
{
	unsigned char t=c/100;
	if(t!=0)
	{
		t+='0';
		parser->writeCallback((const char*)&t,1);
		c-=c%100;
	}
	t=c/10;
	if(t!=0)
	{
		t+='0';
		parser->writeCallback((const char*)&t,1);
		c-=c%10;
	}
	c+='0';
	parser->writeCallback((const char*)&c,1);
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
