#include "TestDeviceState.h"

TestDeviceState::TestDeviceState()
{
}

void TestDeviceState::prepareCommandParamState(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value)
{
	state.commands[cmd][paramIndex]=value;
}

void TestDeviceState::prepareAdditionalParameter(const QByteArray &name,const QByteArray &value)
{
	state.additionalParams[name]=value;
}

void TestDeviceState::setCommandParamState(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value)
{
	state.commands[cmd][paramIndex]=value;
	notifyCommandParamChanged(cmd,paramIndex);
}

void TestDeviceState::setAdditionalParamState(const QByteArray &name,const QByteArray &value)
{
	state.additionalParams[name]=value;
	notifyAdditionalParamChanged(name);
}

void TestDeviceState::dump(Message &m)
{
	for(auto i=state.commands.begin();i!=state.commands.end();++i)
	{
		auto &t=i.value();
		for(auto j=t.begin();j!=t.end();++j)
			writeCommandParamState(i.key(),j.key(),m);
	}
	for(auto i=state.additionalParams.begin();i!=state.additionalParams.end();++i)
		writeAdditionalParamState(i.key(),m);
}

void TestDeviceState::notifyCommandParamChanged(const QByteArray &cmd,quint32 paramIndex)
{
	Message m;
	m.title="statechanged";
	writeCommandParamState(cmd,paramIndex,m);
	emit writeMsg(m);
}

void TestDeviceState::writeCommandParamState(const QByteArray &cmd,quint32 paramIndex,Message &m)
{
	m.args.append(cmd);
	m.args.append(QByteArray::number(paramIndex));
	m.args.append(state.commands.value(cmd).value(paramIndex));
}

void TestDeviceState::notifyAdditionalParamChanged(const QByteArray &name)
{
	Message m;
	m.title="statechanged";
	writeAdditionalParamState(name,m);
	emit writeMsg(m);
}

void TestDeviceState::writeAdditionalParamState(const QByteArray &name,Message &m)
{
	m.args.append("#");
	m.args.append(name);
	m.args.append(state.additionalParams.value(name));
}
