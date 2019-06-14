package com.alterozoom.libwliotproxy;

public class WLIOTServerProtocolDefs
{
	public final static ByteArray authenticateSrvMsg=ByteArray.fromString("authenticate");
	public final static ByteArray srvCmdDataMsg=ByteArray.fromString("cmdata");
	public final static ByteArray notifyDeviceIdentifiedMsg=ByteArray.fromString("device_identified");
	public final static ByteArray notifyDeviceLostMsg=ByteArray.fromString("device_lost");
	public final static ByteArray notifyStorageCreatedMsg=ByteArray.fromString("storage_created");
	public final static ByteArray notifyStorageRemovedMsg=ByteArray.fromString("storage_removed");
	public final static ByteArray notifyReloadDevsAndStorsMsg=ByteArray.fromString("reload_devices_and_storages");
	public final static ByteArray vdevMsg=ByteArray.fromString("vdev");
	public final static int controlSslPort=4083;
}
