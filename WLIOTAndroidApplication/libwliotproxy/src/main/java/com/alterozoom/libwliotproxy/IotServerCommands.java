package com.alterozoom.libwliotproxy;

public class IotServerCommands
{
	private IotServerDevicesCommands devCmds;
	private IotServerStoragesCommands storCmds;

	IotServerCommands(IotServerConnection conn)
	{
		devCmds=new IotServerDevicesCommands(conn);
		storCmds=new IotServerStoragesCommands(conn);
	}

	public IotServerStoragesCommands storages()
	{
		return storCmds;
	}

	public IotServerDevicesCommands devices()
	{
		return devCmds;
	}
}
