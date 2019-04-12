package com.alterozoom.libwliotproxy;

import java.util.ArrayList;

public class ArrayListBuilder<T>
{
	private ArrayList<T> arr;

	public ArrayListBuilder()
	{
		arr=new ArrayList<T>();
	}

	public ArrayListBuilder(ArrayList<T> l)
	{
		arr=l;
	}

	public ArrayListBuilder<T> add(T t)
	{
		arr.add(t);
		return this;
	}

	public ArrayListBuilder<T> add(ArrayList<T> t)
	{
		arr.addAll(t);
		return this;
	}

	public ArrayList<T> done()
	{
		return arr;
	}
}
