package com.alterozoom.libwliotproxy;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Proxy;
import java.security.SecureRandom;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import javax.net.ssl.HandshakeCompletedEvent;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
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

	public static class EventsCallback
	{
		void onPreconnected(){}
		void onConnected(){}
		void onDisconnected(){}
		void needAuthentification(){}
		void connectionError(){}
		void newSensorValue(StorageId id,ArrayList<byte[]> valueArgs){}
		void deviceLost(UUID id){}
		void deviceStateChanged(UUID id,ArrayList<byte[]> args){}
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

	public boolean authenticateNet(byte[] userName,byte[] pass)
	{
		if(!isConnected())
			return false;
		ArrayList<byte[]> args=new ArrayList<>();
		args.add(userName);
		args.add(pass);
		if(!execCommand(WLIOTServerProtocolDefs.authenticateSrvMsg,
			new ArrayListBuilder<>(new ArrayList<byte[]>()).add(userName).add(pass).done()))return false;
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

	public boolean execCommand(byte[] cmd,ArrayList<byte[]> args,ArrayList<byte[]> retVal,CmDataCallback onCmData)
	{
		IotServerCommandCall call=new IotServerCommandCall(this,onCmData,cmd,args);
		retVal=call.returnValue();
		return call.ok();
	}

	public boolean execCommand(byte[] cmd,ArrayList<byte[]> args,CmDataCallback onCmData)
	{
		return execCommand(cmd,args,null,onCmData);
	}

	public boolean execCommand(byte[] cmd,ArrayList<byte[]> args,ArrayList<byte[]> retVal)
	{
		return execCommand(cmd,args,retVal,null);
	}

	public boolean execCommand(byte[] cmd,ArrayList<byte[]> args)
	{
		return execCommand(cmd,args,null,null);
	}

	public boolean execCommand(byte[] cmd)
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

	public boolean subscribeStorage(byte[] devIdOrName,byte[] sensorName)
	{
		return execCommand(StrHelper.toByteArray("subscribe"),
			new ArrayListBuilder<byte[]>().add(devIdOrName).add(sensorName).done());
	}

	public boolean unsubscribeStorage(byte[] devIdOrName,byte[] sensorName)
	{
		return execCommand(StrHelper.toByteArray("unsubscribe"),
			new ArrayListBuilder<byte[]>().add(devIdOrName).add(sensorName).done());
	}

	public boolean identifyServer(UUID id,byte[] name)
	{
		ArrayList<byte[]> retVal=new ArrayList<>();
		if(!execCommand(WLIOTProtocolDefs.identifyMsg,new ArrayList<byte[]>(),retVal)||retVal.size()<2)
			return false;
		id=UUID.fromString(StrHelper.fromByteArray(retVal.get(0)));
		name=retVal.get(1);
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
			new ArrayListBuilder<byte[]>().add(StrHelper.toByteArray(id.toString())).add(m.title).add(m.args).done()));
	}
}
