package com.alterozoom.libwliotproxy;

import java.util.UUID;

public class StorageId
{
	public UUID devId;
	public ByteArray sensorName;

	StorageId(final UUID dId,final ByteArray sName)
	{
		devId=UUID.fromString(dId.toString());
		sensorName=new ByteArray(sName);
	}
}
