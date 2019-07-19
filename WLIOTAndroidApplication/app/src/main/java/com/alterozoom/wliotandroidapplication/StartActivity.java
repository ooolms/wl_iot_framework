package com.alterozoom.wliotandroidapplication;

import android.support.v4.app.Fragment;

public class StartActivity
	extends SingleFragmentActivity
{
	//PreferenceManager.getDefaultSharedPreferences(context)

	@Override
	protected Fragment createFragment()
	{
		return new ConnectToServerFragment();
	}
}
