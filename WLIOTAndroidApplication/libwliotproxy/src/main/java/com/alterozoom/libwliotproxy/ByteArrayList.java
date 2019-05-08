package com.alterozoom.libwliotproxy;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Arrays;

public class ByteArrayList
{
	private ArrayList<ByteArray> data;

	ByteArrayList()
	{
		data=new ArrayList<>();
	}

	ByteArrayList(ByteArrayList t)
	{
		for(ByteArray s:t.data)
			data.add(new ByteArray(s));
	}

	public ArrayList<ByteArray> dataRef()
	{
		return data;
	}

	@Override
	public int hashCode()
	{
		return data.hashCode();
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
		ByteArrayList other=(ByteArrayList)obj;
		return data.equals(other.data);
	}

	public void add(ByteArray a)
	{
		data.add(new ByteArray(a));
	}
}
