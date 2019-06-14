package com.alterozoom.libwliotproxy;

public class ObjectRef<T>
{
	private T t;

	public ObjectRef()
	{}

	public ObjectRef(T o)
	{
		t=o;
	}

	public T o()
	{
		return t;
	}

	public void set(T o)
	{
		t=o;
	}
}
