package com.alterozoom.libwliotproxy;

public class IotServer
{
	private IotServerConnection srvConn;
	private IotServerCommands cmds;

	public IotServer()
	{
		srvConn=new IotServerConnection();
		cmds=new IotServerCommands(srvConn);
	}

	public IotServerConnection connection()
	{
		return srvConn;
	}

	public IotServerCommands commands()
	{
		return cmds;
	}
}
