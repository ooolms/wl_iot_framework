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

class ARpcRealDeviceMessageDispatch;

class ARpcDeviceState
{
private:
	class CommandState
	{
	public:
		CommandState();
		~CommandState();
		void prepare(const char *cmd,unsigned char pCount);
		void updateValue(unsigned char index,const char *val);

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
	explicit ARpcDeviceState(ARpcRealDeviceMessageDispatch *p);
	~ARpcDeviceState();
	void prepareCommands(unsigned char count);
	void prepareCommand(unsigned char index,const char *commandName,unsigned char paramsCount);
	void prepareAdditionalParameters(unsigned char count);
	void prepareAdditionalParameter(unsigned char index,const char *name);
	void setCommandParamState(unsigned char commandIndex,unsigned char paramIndex,const char *value);
	void setAdditionalParamState(unsigned char index,const char *value);
	void dump();

private:
	void notifyCommandParamChanged(unsigned char commandIndex,unsigned char paramIndex);
	void writeCommandParamState(unsigned char commandIndex,unsigned char paramIndex);
	void notifyAdditionalParamChanged(unsigned char index);
	void writeAdditionalParamState(unsigned char index);
	void writeUChar(unsigned char c);

private:
	ARpcRealDeviceMessageDispatch *disp;
	CommandState *mCommands;
	unsigned char mCommandsCount;
	AdditionalParamState *mAddParams;
	unsigned char mAddParamsCount;
};

#endif // ARPCDEVICESTATE_H
