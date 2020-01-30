#include "DeviceState.h"

DeviceState::DeviceState()
{
}

void DeviceState::prepareCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	state.commands[cmd][paramIndex]=value;
}

void DeviceState::prepareAdditionalParameter(const QByteArray &name,const QByteArray &value)
{
	state.additionalParams[name]=value;
}

void DeviceState::setCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	state.commands[cmd][paramIndex]=value;
	notifyCommandParamChanged(cmd,paramIndex);
}

void DeviceState::setAdditionalParamState(const QByteArray &name,const QByteArray &value)
{
	state.additionalParams[name]=value;
	notifyAdditionalParamChanged(name);
}

void DeviceState::dump(Message &m)
{
	for(auto i=state.commands.begin();i!=state.commands.end();++i)
	{
		QMap<int,QByteArray> &t=i.value();
		for(auto j=t.begin();j!=t.end();++j)
			writeCommandParamState(i.key(),j.key(),m);
	}
	for(auto i=state.additionalParams.begin();i!=state.additionalParams.end();++i)
		writeAdditionalParamState(i.key(),m);
}

void DeviceState::notifyCommandParamChanged(const QByteArray &cmd,int paramIndex)
{
	Message m;
	m.title="statechanged";
	writeCommandParamState(cmd,paramIndex,m);
	emit writeMsg(m);
}

void DeviceState::writeCommandParamState(const QByteArray &cmd,int paramIndex,Message &m)
{
	m.args.append(cmd);
	m.args.append(QByteArray::number(paramIndex+1));
	m.args.append(state.commands.value(cmd).value(paramIndex));
}

void DeviceState::notifyAdditionalParamChanged(const QByteArray &name)
{
	Message m;
	m.title="statechanged";
	writeAdditionalParamState(name,m);
	emit writeMsg(m);
}

void DeviceState::writeAdditionalParamState(const QByteArray &name,Message &m)
{
	m.args.append("#");
	m.args.append(name);
	m.args.append(state.additionalParams.value(name));
}
