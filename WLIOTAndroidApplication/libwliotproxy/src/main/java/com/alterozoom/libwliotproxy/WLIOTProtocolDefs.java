package com.alterozoom.libwliotproxy;

public class WLIOTProtocolDefs
{
	public final static byte[] funcCallMsg=StrHelper.toBA("call");
	public final static byte[] funcAnswerOkMsg=StrHelper.toBA("ok");
	public final static byte[] funcAnswerErrMsg=StrHelper.toBA("err");
	public final static byte[] devSyncMsg=StrHelper.toBA("sync");
	public final static byte[] devSyncrMsg=StrHelper.toBA("syncr");
	public final static byte[] infoMsg=StrHelper.toBA("info");
	public final static byte[] measurementMsg=StrHelper.toBA("meas");
	public final static byte[] measurementBMsg=StrHelper.toBA("measb");
	public final static byte[] identifyMsg=StrHelper.toBA("identify");
	public final static byte[] identifyHubMsg=StrHelper.toBA("identify_hub");
	public final static byte[] deviceInfoMsg=StrHelper.toBA("deviceinfo");
	public final static byte[] stateChangedMsg=StrHelper.toBA("statechanged");
	public final static byte[] hubMsg=StrHelper.toBA("#hub";
	public final static byte[] hubDeviceIdentifiedMsg=StrHelper.toBA("device_identified");
	public final static byte[] hubDeviceLostMsg=StrHelper.toBA("device_lost");
//	public final static QString queryVersionMsg;
//	public final static QString versionMsg;
	public final static byte msgDelim=10;
	public final static byte argDelim=124;
	public final static int identifyWaitTime=5000;//msecs
	public final static int syncWaitTime=5000;//msecs
	public final static byte[] serverReadyMsg=StrHelper.toBA("server_ready");

	//reserved commands
	public final static byte[] getSensorsCommand=StrHelper.toBA("#sensors");
	public final static byte[] getControlsCommand=StrHelper.toBA("#controls");
	public final static byte[] getStateCommand=StrHelper.toBA("#state");

	public final static int netDevicePort=4081;
	public final static int netDeviceSslPort=4082;
}
