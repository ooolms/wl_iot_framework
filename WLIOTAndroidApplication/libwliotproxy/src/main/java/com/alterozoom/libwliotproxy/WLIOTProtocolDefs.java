package com.alterozoom.libwliotproxy;

public class WLIOTProtocolDefs
{
	public final static ByteArray funcCallMsg=ByteArray.fromString("call");
	public final static ByteArray funcAnswerOkMsg=ByteArray.fromString("ok");
	public final static ByteArray funcAnswerErrMsg=ByteArray.fromString("err");
	public final static ByteArray devSyncMsg=ByteArray.fromString("sync");
	public final static ByteArray devSyncrMsg=ByteArray.fromString("syncr");
	public final static ByteArray infoMsg=ByteArray.fromString("info");
	public final static ByteArray measurementMsg=ByteArray.fromString("meas");
	public final static ByteArray measurementBMsg=ByteArray.fromString("measb");
	public final static ByteArray identifyMsg=ByteArray.fromString("identify");
	public final static ByteArray identifyHubMsg=ByteArray.fromString("identify_hub");
	public final static ByteArray deviceInfoMsg=ByteArray.fromString("deviceinfo");
	public final static ByteArray stateChangedMsg=ByteArray.fromString("statechanged");
	public final static ByteArray hubMsg=ByteArray.fromString("#hub");
	public final static ByteArray hubDeviceIdentifiedMsg=ByteArray.fromString("device_identified");
	public final static ByteArray hubDeviceLostMsg=ByteArray.fromString("device_lost");
//	public final static QString queryVersionMsg;
//	public final static QString versionMsg;
	public final static byte msgDelim=10;
	public final static byte argDelim=124;
	public final static int identifyWaitTime=5000;//msecs
	public final static int syncWaitTime=5000;//msecs
	public final static ByteArray serverReadyMsg=ByteArray.fromString("server_ready");

	//reserved commands
	public final static ByteArray getSensorsCommand=ByteArray.fromString("#sensors");
	public final static ByteArray getControlsCommand=ByteArray.fromString("#controls");
	public final static ByteArray getStateCommand=ByteArray.fromString("#state");

	public final static int netDevicePort=4081;
	public final static int netDeviceSslPort=4082;
}
