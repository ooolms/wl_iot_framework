package com.alterozoom.libwliotproxy;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;

public class ByteArray
{
	private byte[] data;
	private static Charset chst=Charset.forName("utf-8");

	ByteArray()
	{
		data=new byte[0];
	}

	ByteArray(byte[] a)
	{
		data=a;
	}

	ByteArray(ByteArray t)
	{
		data=t.data.clone();
	}

	public byte[] dataRef()
	{
		return data;
	}

	@Override
	public int hashCode()
	{
		return Arrays.hashCode(data);
	}

	@Override
	public boolean equals(Object obj)
	{
		if(this==obj)
			return true;
		if(obj==null)
			return false;
		if(getClass()!=obj.getClass())
			return false;
		ByteArray other=(ByteArray)obj;
		return Arrays.equals(data,other.data);
	}

	public static byte[] toBA(String s)
	{
		return chst.encode(s).array();
	}

	public static ByteArray fromString(String s)
	{
		return new ByteArray(chst.encode(s).array());
	}

	public static String fromBA(byte[] s)
	{
		return chst.decode(ByteBuffer.wrap(s)).toString();
	}

	@Override
	public String toString()
	{
		return chst.decode(ByteBuffer.wrap(data)).toString();
	}

	public static ByteArrayList split(byte sep)
	{
		ByteArrayOutputStream s=new ByteArrayOutputStream();
		ByteArrayList list=new ByteArrayList();
	}
}
