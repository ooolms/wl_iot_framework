#include "DeviceState.h"
#include "ARpcStreamWriter.h"

DeviceState::DeviceState(ARpcStreamWriter *wr)
{
	writer=wr;
}

void DeviceState::prepareCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	m.commands[cmd][paramIndex]=value;
}

void DeviceState::prepareAdditionalParameter(const QByteArray &name,const QByteArray &value)
{
	m.additionalParams[name]=value;
}

void DeviceState::setCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	m.commands[cmd][paramIndex]=value;
	notifyCommandParamChanged(cmd,paramIndex);
}

void DeviceState::setAdditionalParamState(const QByteArray &name,const QByteArray &value)
{
	m.additionalParams[name]=value;
	notifyAdditionalParamChanged(name);
}

void DeviceState::dump()
{
	for(auto i=m.commands.begin();i!=m.commands.end();++i)
	{
		QMap<int,QByteArray> &t=i.value();
		for(auto j=t.begin();j!=t.end();++j)
			writeCommandParamState(i.key(),j.key());
	}
	for(auto i=m.additionalParams.begin();i!=m.additionalParams.end();++i)
		writeAdditionalParamState(i.key());
}

void DeviceState::notifyCommandParamChanged(const QByteArray &cmd,int paramIndex)
{
	if(!writer->beginWriteMsg())return;
	writer->writeArgNoEscape("statechanged");
	writeCommandParamState(cmd,paramIndex);
	writer->endWriteMsg();
}

void DeviceState::writeCommandParamState(const QByteArray &cmd,int paramIndex)
{
	writer->writeArg(cmd.constData(),cmd.size());
	writeUInt(paramIndex+1);
	QByteArray v=m.commands.value(cmd).value(paramIndex);
	writer->writeArg(v.constData(),v.size());
}

void DeviceState::notifyAdditionalParamChanged(const QByteArray &name)
{
	if(!writer->beginWriteMsg())return;
	writer->writeArgNoEscape("statechanged");
	writeAdditionalParamState(name);
	writer->endWriteMsg();
}

void DeviceState::writeAdditionalParamState(const QByteArray &name)
{
	writer->writeArgNoEscape("#");
	QByteArray v=m.additionalParams.value(name);
	writer->writeArg(name.constData(),name.size());
	writer->writeArg(v.constData(),v.size());
}

void DeviceState::writeUInt(unsigned int c)
{
	QByteArray v=QByteArray::number(c);
	writer->writeArgNoEscape(v.constData());
}
