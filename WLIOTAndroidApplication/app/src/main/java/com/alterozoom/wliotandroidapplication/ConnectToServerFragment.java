package com.alterozoom.wliotandroidapplication;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.alterozoom.libwliotproxy.ByteArray;
import com.alterozoom.libwliotproxy.IotServerConnection;

public class ConnectToServerFragment
	extends Fragment
{
	private TextView statusBar;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setRetainInstance(true);
		setHasOptionsMenu(true);
	}

	@Override
	public View onCreateView(LayoutInflater inflater,ViewGroup container,Bundle savedInstanceState)
	{
		View v=inflater.inflate(R.layout.connect_to_server_fragment,container,false);
		statusBar=(TextView)v.findViewById(R.id.connect_to_server_fragment_status_view);
		Button mBtn=(Button)v.findViewById(R.id.connect_to_server_fragment_connect_btn);
		mBtn.setOnClickListener(new View.OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				String host=((EditText)v.findViewById(R.id.connect_to_server_fragment_host_edit)).getText().toString();
				String user=((EditText)v.findViewById(R.id.connect_to_server_fragment_user_edit)).getText().toString();
				String pass=((EditText)v.findViewById(R.id.connect_to_server_fragment_pass_edit)).getText().toString();
				if(!S.srv.connection().startConnectNet(host,IotServerConnection.defaultPort))
					statusBar.setText("connection failed");
				else if(!S.srv.connection().authenticateNet(ByteArray.fromString(user),ByteArray.fromString(pass)))
					statusBar.setText("authentication failed");
				else statusBar.setText("connection successful");
			}
		});
		return v;
	}
}
