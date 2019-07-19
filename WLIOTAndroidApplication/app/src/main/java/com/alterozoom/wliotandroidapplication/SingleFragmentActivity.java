package com.alterozoom.wliotandroidapplication;

import android.os.Bundle;
import android.support.annotation.LayoutRes;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.ActionBarActivity;

public abstract class SingleFragmentActivity
	extends ActionBarActivity
{
	protected abstract Fragment createFragment();

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(getLayoutResId());

		FragmentManager fm=getSupportFragmentManager();
		Fragment fragment=fm.findFragmentById(R.id.fragment_container);
		if(fragment==null)
		{
			fragment=createFragment();
			fm.beginTransaction().add(R.id.fragment_container,fragment).commit();
		}
	}

	protected void replaceFragment()
	{
		FragmentManager fm=getSupportFragmentManager();
		Fragment fragment=fm.findFragmentById(R.id.fragment_container);
		if(fragment!=null)
		{
			fm.beginTransaction().remove(fragment).commit();
			fragment=createFragment();
			fm.beginTransaction().add(R.id.fragment_container,fragment).commit();
		}
	}

	@LayoutRes
	protected int getLayoutResId()
	{
		return R.layout.single_fragment_activity;
	}
}
