package com.alterozoom.libwliotproxy;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class Message
{
	public ByteArray title;
	public ByteArrayList args;

	Message()
	{
	}

	Message(ByteArray t)
	{
		title=t;
	}

	Message(ByteArray t,ByteArrayList a)
	{
		title=t;
		args=a;
	}

	Message(Message t)
	{
		title=new ByteArray(t.title);
		args=new ByteArrayList(t.args);
	}

	static ByteArray escape(ByteArray data)
	{
		ByteArrayOutputStream b=new ByteArrayOutputStream();
		byte[] bytes=data.dataRef();
		try
		{
			for(int i=0;i<bytes.length;++i)
			{
				if(bytes[i]=='\\')
					b.write(ByteArray.toBA("\\\\"));
				else if(bytes[i]=='|')
					b.write(ByteArray.toBA("\\|"));
				else if(bytes[i]=='\n')
					b.write(ByteArray.toBA("\\n"));
				else if(bytes[i]==0)
					b.write(ByteArray.toBA("\\0"));
				else b.write(bytes[i]);
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
			return new ByteArray();
		}
		return new ByteArray(b.toByteArray());
	}

	byte[] dump()
	{
		return dump(title,args);
	}

	static byte[] dump(ByteArray msg,ByteArrayList args)
	{
		ByteArrayOutputStream b=new ByteArrayOutputStream();
		try
		{
			b.write(escape(msg).dataRef());
			for(int i=0;i<args.size();++i)
			{
				b.write(WLIOTProtocolDefs.argDelim);
				b.write(escape(args.get(i)).dataRef());
			}
			b.write(WLIOTProtocolDefs.msgDelim);
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		return b.toByteArray();
	}
}
