package com.alterozoom.libwliotproxy;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Proxy;
import java.security.SecureRandom;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.UUID;

import javax.net.ssl.HandshakeCompletedEvent;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

//TODO rewrite network read/write with multithreading and blocking queues for read and write messages

public class IotServerConnection
{
	private SSLSocket netSock;
	private InputStream iStream;
	private OutputStream oStream;
	private Thread readThread,writeThread;
	private long callIdNum;
	private boolean netAuthentificated;
	private Proxy proxy;
	private StreamParser parser;
	private ArrayList<EventsCallback> callbacks;
	private SSLSocketFactory sockFactory;
	private boolean wasSyncMsg;
	private StreamParserCb parserCb;
	public static final int defaultPort=4083;

	private class StreamParserCb
		extends StreamParser.StreamParserEventsCb
	{
		public void onNewMessage(Message m)
		{
			if(m.title.equals(WLIOTProtocolDefs.devSyncMsg))
			{
				wasSyncMsg=true;
				writeMsg(new Message(WLIOTProtocolDefs.devSyncrMsg));
			}
			else if(m.title.equals(WLIOTProtocolDefs.measurementMsg))
			{
				if(m.args.size()<3)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				ByteArray sensorName=m.args.get(1);
				if(devId==null)return;
				StorageId stId=new StorageId(devId,sensorName);
				ByteArrayList args=new ByteArrayList(m.args);
				args.remove(0);
				args.remove(0);
				for(EventsCallback cb:callbacks)
					cb.newSensorValue(stId,args);
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.notifyDeviceIdentifiedMsg))
			{
				if(m.args.size()<2)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				ByteArray devName=m.args.get(1);
				UUID typeId=null;
				if(m.args.size()>2)
					typeId=UUID.fromString(m.args.get(2).toString());
				if(devId==null)return;
				for(EventsCallback cb:callbacks)
					cb.deviceIdentified(devId,devName,typeId);
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.notifyDeviceLostMsg))
			{
				if(m.args.size()<1)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				if(devId==null)return;
				for(EventsCallback cb:callbacks)
					cb.deviceLost(devId);
			}
			else if(m.title.equals(WLIOTProtocolDefs.stateChangedMsg))
			{
				if(m.args.size()<1)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				ByteArrayList args=new ByteArrayList(m.args);
				args.remove(0);
				if(args.size()%3!=0)return;
				for(EventsCallback cb:callbacks)
					cb.deviceStateChanged(devId,args);
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.notifyStorageCreatedMsg))
			{
				IotServerStorageDescr st=new IotServerStorageDescr();
				if(!IotServerStoragesCommands.storageFromArgs(m.args,st))return;
				for(EventsCallback cb:callbacks)
					cb.storageCreated(st);
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.notifyStorageRemovedMsg))
			{
				if(m.args.size()<2)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				ByteArray sensorName=m.args.get(1);
				if(devId==null)return;
				StorageId stId=new StorageId(devId,sensorName);
				for(EventsCallback cb:callbacks)
					cb.storageRemoved(stId);
			}
			else if(m.title.equals(WLIOTServerProtocolDefs.vdevMsg))
			{
				if(m.args.size()<2)return;
				UUID devId=UUID.fromString(m.args.get(0).toString());
				if(devId==null)return;
				ByteArray devMsg=m.args.get(1);
				ByteArrayList args=new ByteArrayList(m.args);
				args.remove(0);
				args.remove(0);
				for(EventsCallback cb:callbacks)
					cb.vdevMsg(devId,new Message(devMsg,args));
			}
			else if(m.title.equals(WLIOTProtocolDefs.funcAnswerOkMsg)||
				m.title.equals(WLIOTProtocolDefs.funcAnswerErrMsg)||
				m.title.equals(WLIOTServerProtocolDefs.srvCmdDataMsg))
			{
				for(EventsCallback cb:callbacks)
					cb.funcCallReplyMsg(m);
			}
		}
	}

	public static class EventsCallback
	{
		void onPreconnected(){}
		void onConnected(){}
		void onDisconnected(){}
		void needAuthentification(){}
		void connectionError(){}
		void newSensorValue(StorageId id,ByteArrayList valueArgs){}
		void deviceIdentified(UUID id,ByteArray name,UUID typeId){}
		void deviceLost(UUID id){}
		void deviceStateChanged(UUID id,ByteArrayList args){}
		void storageCreated(IotServerStorageDescr s){}
		void storageRemoved(StorageId id){}
		void funcCallReplyMsg(Message m){}
		void vdevMsg(UUID id,Message m){}
	}

	public IotServerConnection()
	{
		callIdNum=0;
		netAuthentificated=false;
		parser=new StreamParser();
		parserCb=new StreamParserCb();
		parser.addCb(parserCb);
		try
		{
			TrustManager[] victimizedManager = new TrustManager[]
			{
				new X509TrustManager()
				{
					public X509Certificate[] getAcceptedIssuers()
					{

						X509Certificate[] myTrustedAnchors=new X509Certificate[0];
						return myTrustedAnchors;
					}

					@Override
					public void checkClientTrusted(X509Certificate[] certs, String authType)
					{
					}

					@Override
					public void checkServerTrusted(X509Certificate[] certs, String authType)
					{
					}
				}
			};

			SSLContext sc=SSLContext.getInstance("SSL");
			sc.init(null,victimizedManager,new SecureRandom());
			sockFactory=sc.getSocketFactory();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void finalize()
	{
		disconnectFromServer();
		parser.removeCb(parserCb);
	}

	public void registerCallback(EventsCallback cb)
	{
		if(callbacks.contains(cb))return;
		callbacks.add(cb);
	}

	public void removeCallback(EventsCallback cb)
	{
		callbacks.remove(cb);
	}

	public boolean startConnectNet(String host,int port)
	{
		if(netSock!=null)return false;
		netAuthentificated=false;
		parser.reset();
		try
		{
			netSock=(SSLSocket)sockFactory.createSocket(InetAddress.getByName(host),port);
			netSock.startHandshake();
			netSock.addHandshakeCompletedListener(new HandshakeCompletedListener(){
				@Override
				public void handshakeCompleted(HandshakeCompletedEvent handshakeCompletedEvent)
				{
					netSock.notify();
				}
			});
			try
			{
				netSock.wait();
			}
			catch(InterruptedException e)
			{
				e.printStackTrace();
			}
			if(!netSock.isConnected())
			{
				netSock=null;
				return false;
			}
			iStream=netSock.getInputStream();
			oStream=netSock.getOutputStream();
			readThread=new Thread(new Runnable(){
				@Override
				public void run()
				{
					try
					{
						int c=iStream.read();
						if(c>0)
							parser.putByte((byte)c);
					}
					catch(IOException e)
					{
						e.printStackTrace();
					}
				}
			});
			readThread.start();
		}
		catch(IOException e)
		{
			e.printStackTrace();
			try
			{
				if(netSock!=null)
				{
					netSock.close();
					netSock=null;
				}
			}
			catch(IOException e1)
			{
				e1.printStackTrace();
			}
			return false;
		}
		return true;
	}

	public boolean authenticateNet(ByteArray userName,ByteArray pass)
	{
		if(!isConnected())
			return false;
		ByteArrayList args=new ByteArrayList();
		args.add(userName);
		args.add(pass);
		if(!execCommand(WLIOTServerProtocolDefs.authenticateSrvMsg,
			new ByteArrayListBuilder().add(userName).add(pass).done()))return false;
		netAuthentificated=true;
		for(EventsCallback cb:callbacks)
			cb.onPreconnected();
		for(EventsCallback cb:callbacks)
			cb.onConnected();
		return true;

	}

	public boolean isConnected()
	{
		if(netSock==null)
			return false;
		return netSock.isConnected();
	}

	public boolean execCommand(
		final ByteArray cmd,final ByteArrayList args,ObjectRef<ByteArrayList> retVal,CmDataCallback onCmData)
	{
		IotServerCommandCall call=new IotServerCommandCall(this,onCmData,cmd,args);
		if(retVal!=null)
			retVal.set(call.returnValue());
		return call.ok();
	}

	public boolean execCommand(final ByteArray cmd,final ByteArrayList args,CmDataCallback onCmData)
	{
		return execCommand(cmd,args,null,onCmData);
	}

	public boolean execCommand(final ByteArray cmd,final ByteArrayList args,ObjectRef<ByteArrayList> retVal)
	{
		return execCommand(cmd,args,retVal,null);
	}

	public boolean execCommand(final ByteArray cmd,final ByteArrayList args)
	{
		return execCommand(cmd,args,null,null);
	}

	public boolean execCommand(final ByteArray cmd)
	{
		return execCommand(cmd,null,null,null);
	}

	public boolean waitForConnected(int msec)
	{
		if(netSock==null)
			return false;
		return netSock.isConnected();
	}

	public void disconnectFromServer()
	{
		if(netSock==null)return;
		try
		{
			netSock.close();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		readThread.interrupt();
		netSock=null;
		iStream=null;
		oStream=null;
		readThread=null;
	}

	public boolean subscribeStorage(final ByteArray devIdOrName,final ByteArray sensorName)
	{
		return execCommand(ByteArray.fromString("subscribe"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done());
	}

	public boolean unsubscribeStorage(final ByteArray devIdOrName,final ByteArray sensorName)
	{
		return execCommand(ByteArray.fromString("unsubscribe"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done());
	}

	public boolean identifyServer(ObjectRef<UUID> id,ObjectRef<ByteArray> name)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!execCommand(WLIOTProtocolDefs.identifyMsg,new ByteArrayList(),retVal)||retVal.o().size()<2)
			return false;
		id.set(UUID.fromString(retVal.o().get(0).toString()));
		name.set(retVal.o().get(1));
		return id!=null;
	}

	public boolean writeMsg(Message m)
	{
		if(netSock==null)
			return false;
		byte[] msgData=m.dump();
		try
		{
			oStream.write(msgData);
			oStream.flush();
		}
		catch(IOException e)
		{
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public void setProxy(Proxy p)
	{
	}

	public boolean writeVDevMsg(UUID id,Message m)
	{
		return writeMsg(new Message(WLIOTServerProtocolDefs.vdevMsg,
			new ByteArrayListBuilder().add(ByteArray.fromString(id.toString())).add(m.title).add(m.args).done()));
	}
}
