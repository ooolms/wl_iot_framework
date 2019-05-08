package com.alterozoom.libwliotproxy;

public class WLIOTServerProtocolDefs
{
	public final static byte[] authenticateSrvMsg=StrHelper.toBA("authenticate");
	public final static byte[] srvCmdDataMsg=StrHelper.toBA("cmdata");
	public final static byte[] notifyDeviceIdentifiedMsg=StrHelper.toBA("device_identified");
	public final static byte[] notifyDeviceLostMsg=StrHelper.toBA("device_lost");
	public final static byte[] notifyStorageCreatedMsg=StrHelper.toBA("storage_created");
	public final static byte[] notifyStorageRemovedMsg=StrHelper.toBA("storage_removed");
	public final static byte[] notifyReloadDevsAndStorsMsg=StrHelper.toBA("reload_devices_and_storages");
	public final static byte[] vdevMsg=StrHelper.toBA("vdev");
	public final static int controlSslPort=4083;
}
