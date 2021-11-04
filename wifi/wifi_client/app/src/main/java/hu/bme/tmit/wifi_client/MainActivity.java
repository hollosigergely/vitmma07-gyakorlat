package hu.bme.tmit.wifi_client;


import android.app.Activity;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.format.Formatter;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends Activity implements OnClickListener
{
	public static String[] APS = {
			"30:AE:A4:97:EC:C9",
			"24:62:AB:D6:EC:99",
			"24:62:AB:DC:B2:55",
			"7C:9E:BD:F4:E1:E9"};
	
	Button measurementButton = null;
	Button calibrationButton = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		
		WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);
		WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
		int ip = wifiInfo.getIpAddress();
		String ipAddress = Formatter.formatIpAddress(ip);
		
		TextView ipText = (TextView)this.findViewById(R.id.iptext);
		ipText.setText("IP address: "+ipAddress);
		
		measurementButton = (Button)this.findViewById(R.id.measurementbutton);
		calibrationButton = (Button)this.findViewById(R.id.calibrationbutton);
		
		measurementButton.setOnClickListener(this);
		calibrationButton.setOnClickListener(this);
	}

	
	@Override
	protected void onResume()
	{
		super.onResume();
		
		checkServiceState();
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
	
	}
	
	boolean checkServiceState()
	{
		if(ServerService.isRunning)
		{
			measurementButton.setText("Stop measurement");
			return true;
		}
		else
		{
			measurementButton.setText("Start measurement");
		}
		
		return false;
	}
	
	@Override
	public void onClick(View v)
	{
		if(v == MainActivity.this.measurementButton)
		{
			if(ServerService.isRunning)
			{
				Intent intent = new Intent(MainActivity.this, ServerService.class);
				stopService(intent);
				measurementButton.setText("Start measurement");
			}
			else
			{
				Intent intent = new Intent(MainActivity.this, ServerService.class);
				startService(intent);
				measurementButton.setText("Stop measurement");
			}
		}
		else if(v == MainActivity.this.calibrationButton)
		{
			Intent intent = new Intent(MainActivity.this, CalibrateActivity.class);
			startActivity(intent);
		}
	}
	
	
	
/*******************************************************************************************/	
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}



}
