package com.alterozoom.libwliotproxy;

public class ByteArrayListBuilder
{
	private ByteArrayList arr;

	public ByteArrayListBuilder()
	{
		arr=new ByteArrayList();
	}

	public ByteArrayListBuilder(ByteArrayList l)
	{
		arr=l;
	}

	public ByteArrayListBuilder add(ByteArray t)
	{
		arr.add(t);
		return this;
	}

	public ByteArrayListBuilder add(ByteArrayList t)
	{
		arr.add(t);
		return this;
	}

	public ByteArrayList done()
	{
		return arr;
	}
}
