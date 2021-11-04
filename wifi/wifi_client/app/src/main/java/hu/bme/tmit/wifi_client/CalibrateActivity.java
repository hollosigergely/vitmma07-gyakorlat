package hu.bme.tmit.wifi_client;


import java.io.IOException;
import java.net.ServerSocket;
import android.app.Activity;
import android.content.Context;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;


public class CalibrateActivity extends AppCompatActivity implements OnTouchListener, ReceiverCallback
{

	private Thread backgroundThread = null;
	private WifiManager wifiManager = null;
	private MyBroadcastReceiver bcr = null;
	
	private CalibrateMapView calibrateMapView = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_calibrate);
		
		calibrateMapView = (CalibrateMapView)findViewById(R.id.calibratemapview);
		calibrateMapView.setOnTouchListener(this);
		
		if(backgroundThread == null || !backgroundThread.isAlive())
		{
			wifiManager = (WifiManager)this.getSystemService(Context.WIFI_SERVICE);			
			bcr = new MyBroadcastReceiver(wifiManager, true, this);
			this.registerReceiver(bcr, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
			
			backgroundThread = new Thread(bcr);
			backgroundThread.start();
		}
	}
	
	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		
		if(backgroundThread != null && backgroundThread.isAlive())
		{
			try {
				bcr.isRunning = false;
				
				this.unregisterReceiver(bcr);
				
				backgroundThread.interrupt();
				backgroundThread.join();
				backgroundThread = null;
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	@Override
	public boolean onTouch(View v, MotionEvent event)
	{
		calibrateMapView.setX(event.getX());
		calibrateMapView.setY(event.getY());
		calibrateMapView.invalidate();
		
		bcr.setCoordinate(event.getX() / calibrateMapView.getWidth(), event.getY() / calibrateMapView.getHeight());
		
		return true;
	}
	
	
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.calibrate, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		int id = item.getItemId();
		
		if(id == R.id.action_start)
		{
			bcr.startCalibration();
			calibrateMapView.setStarted(true);
			calibrateMapView.invalidate();
			return true;
		}
		else if(id == R.id.action_stop)
		{
			bcr.stopCalibration();
			calibrateMapView.setStarted(false);
			calibrateMapView.invalidate();
			return true;
		}
		else if(id == R.id.action_save)
		{
			boolean isSaved = bcr.saveCalibrationDatas();
			
			if(isSaved)
				Toast.makeText(getApplicationContext(), "Calibration saved", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(getApplicationContext(), "Calibration saving failed :(", Toast.LENGTH_SHORT).show();
			
			return true;
		}
		else if(id == R.id.action_clear)
		{
			bcr.clearCalibrationDatas();
			calibrateMapView.setCalibDataNum(bcr.getCalibrationDatas().size());
			calibrateMapView.invalidate();
			return true;
		}
		
		return super.onOptionsItemSelected(item);
	}
	
	@Override
	public void callback()
	{
		calibrateMapView.setCalibDataNum(bcr.getCalibrationDatas().size());
		calibrateMapView.invalidate();
	}
}
