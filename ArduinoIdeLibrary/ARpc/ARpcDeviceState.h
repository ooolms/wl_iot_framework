#ifndef ARPCDEVICESTATE_H
#define ARPCDEVICESTATE_H

#include "ARpcBase.h"

class ARpc;

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
	explicit ARpcDeviceState(ARpc *p);
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
	ARpc *parser;
	CommandState *mCommands;
	unsigned char mCommandsCount;
	AdditionalParamState *mAddParams;
	unsigned char mAddParamsCount;
};

#endif // ARPCDEVICESTATE_H
