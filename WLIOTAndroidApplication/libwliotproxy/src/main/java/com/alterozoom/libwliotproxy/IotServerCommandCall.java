package com.alterozoom.libwliotproxy;

import java.util.Timer;
import java.util.TimerTask;

public class IotServerCommandCall
{
	private class ConnEvCb
		extends IotServerConnection.EventsCallback
	{
		void onDisconnected()
		{
			retVal.add(ByteArray.fromString("server disconnected"));
			waitObj.notify();
		}

		void funcCallReplyMsg(Message m)
		{
			if(m.args.size()==0)return;
			if(m.title.equals(WLIOTProtocolDefs.funcAnswerOkMsg))
			{
				if(!m.args.get(0).equals(callId))return;
				mOk=true;
				retVal=new ByteArrayList(m.args);
				retVal.remove(0);
				waitObj.notify();
			}
			else if(m.title.equals(WLIOTProtocolDefs.funcAnswerErrMsg))
			{
				if(!m.args.get(0).equals(callId))return;
				retVal=new ByteArrayList(m.args);
				retVal.remove(0);
				waitObj.notify();
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.srvCmdDataMsg))
			{
				if(!m.args.get(0).equals(callId)||mOnCmData==null)return;
				ByteArrayList lCopy=new ByteArrayList(m.args);
				lCopy.remove(0);
				if(!mOnCmData.onCmData(lCopy))
					waitObj.notify();
			}
		}
	}

	private ConnEvCb evCb;
	private IotServerConnection srvConn;
	private CmDataCallback mOnCmData;
	private ByteArray mCmd;
	private ByteArrayList mArgs;
	private ByteArrayList retVal;
	private ByteArray callId;
	private boolean mOk;
	private static long callIdNum=0;
	private Object waitObj;
	private Timer tmr;

	public IotServerCommandCall(IotServerConnection conn,CmDataCallback onCmData,final ByteArray cmd,
		final ByteArrayList args)
	{
		srvConn=conn;
		mOnCmData=onCmData;
		mCmd=cmd;
		mArgs=args;
		evCb=new ConnEvCb();
		srvConn.registerCallback(evCb);
		callId=ByteArray.fromString(Long.toString(++callIdNum));
		tmr=new Timer();
		waitObj=new Object();
		mOk=false;
	}

	public void finalize()
	{
		srvConn.removeCallback(evCb);
	}

	public void call()
	{
		if(!srvConn.isConnected())
		{
			mOk=false;
			retVal.add(ByteArray.fromString("disconnected"));
			return;
		}
		if(mArgs==null)
			srvConn.writeMsg(new Message(mCmd,new ByteArrayListBuilder().add(callId).done()));
		else srvConn.writeMsg(new Message(mCmd,new ByteArrayListBuilder().add(callId).add(mArgs).done()));
		tmr.schedule(new TimerTask(){
			@Override
			public void run()
			{
				retVal.add(ByteArray.fromString("timeout"));
				mOk=false;
				waitObj.notify();
			}
		},WLIOTProtocolDefs.syncWaitTime*4);
		try
		{
			waitObj.wait();
		}
		catch(InterruptedException e)
		{
			e.printStackTrace();
			mOk=false;
			retVal.add(ByteArray.fromString("exception catched"));
		}
	}

	boolean ok()
	{
		return mOk;
	}

	ByteArrayList returnValue()
	{
		return retVal;
	}
}
