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

#ifndef ARPCDEVICESTATE_H
#define ARPCDEVICESTATE_H

class ARpcStreamWriter;

//TODO вынести state из msgdisp, чтобы можно было использовать один state на все интерфейсы

class ARpcDeviceState
{
private:
	class CommandState
	{
	public:
		CommandState();
		~CommandState();
		void prepare(const char *cmd,unsigned int pCount);
		void updateValue(unsigned int index,const char *val);

	public:
		char *command;
		unsigned char paramsCount;
		char **paramsValues;
	};

	class AdditionalParamState
	{
	public:
		AdditionalParamState();
		~AdditionalParamState();
		void prepare(const char *name);
		void updateValue(const char *val);

	public:
		char *paramName;
		char *paramValue;
	};

public:
	explicit ARpcDeviceState(ARpcStreamWriter *wr);
	~ARpcDeviceState();
	void prepareCommands(unsigned int count);
	void prepareCommand(unsigned int index,const char *commandName,unsigned int paramsCount);
	void prepareAdditionalParameters(unsigned int count);
	void prepareAdditionalParameter(unsigned int index,const char *name);
	void setCommandParamState(unsigned int commandIndex,unsigned int paramIndex,const char *value);
	void setAdditionalParamState(unsigned int index,const char *value);
	void dump();
	void writeUInt(unsigned int c);//works up to 32 bit int

private:
	void notifyCommandParamChanged(unsigned int commandIndex,unsigned int paramIndex);
	void writeCommandParamState(unsigned int commandIndex,unsigned int paramIndex);
	void notifyAdditionalParamChanged(unsigned int index);
	void writeAdditionalParamState(unsigned int index);

private:
	ARpcStreamWriter *writer;
	CommandState *mCommands;
	unsigned int mCommandsCount;
	AdditionalParamState *mAddParams;
	unsigned int mAddParamsCount;
};

#endif // ARPCDEVICESTATE_H
