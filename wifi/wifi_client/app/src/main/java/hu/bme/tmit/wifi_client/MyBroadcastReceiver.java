package hu.bme.tmit.wifi_client;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.LinkedList;
import java.util.List;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.util.Log;

interface ReceiverCallback
{
	public void callback();
}

public class MyBroadcastReceiver extends BroadcastReceiver implements Runnable
{
	private ReceiverCallback callback = null;
	private WifiManager wifiManager = null;
	public boolean isRunning = true;
	private List<ScanResult> scanResults = new LinkedList<ScanResult>();
	
	private List<CalibrationData> calibrationDatas = new LinkedList<CalibrationData>();
	private boolean isCalibration = false;
	private boolean isStarted = false;
	private float x = 0;
	private float y = 0;
	
	
	MyBroadcastReceiver(WifiManager wifiManager, boolean isCalibration, ReceiverCallback callback)
	{
		this.wifiManager = wifiManager;
		this.isCalibration = isCalibration;
		this.callback = callback;
	}
	
	public List<ScanResult> getScanResults()
	{
		return scanResults;
	}
	
	public List<CalibrationData> getCalibrationDatas()
	{
		return calibrationDatas;
	}

	public void startCalibration()
	{
		isStarted = true;
	}
	
	public void stopCalibration()
	{
		isStarted = false;
	}
	
	public void clearCalibrationDatas()
	{
		synchronized(calibrationDatas)
		{
			calibrationDatas.clear();	
		}
	}
	
	public void setCoordinate(float x, float y)
	{
		this.x = x;
		this.y = y;
	}
	
	private int getDbmByMac(List<ScanResult> scanResults, String mac)
	{
		for(ScanResult result : scanResults)
		{
			if(result.BSSID.equalsIgnoreCase(mac))
				return result.level;
		}
		
		return 0;
	}
	
	public boolean saveCalibrationDatas()
	{
		try {
			String calibrationDirPath = "/sdcard/calib_datas/calib_";
			
			File calibFile = new File(calibrationDirPath+System.currentTimeMillis()+".calib");
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(calibFile, true)));
			
			out.print("A = [");
			
			synchronized(calibrationDatas)
			{
				for(CalibrationData data : calibrationDatas)
				{
					out.print(data.x+" "+data.y+" ");
					for(String mac : MainActivity.APS)
					{
						out.print(getDbmByMac(data.scanResults, mac)+" ");
					}
					out.println(";");
				}
			}
						
			out.println("];");
			out.close();
			
			return true;
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		
		return false;
	}

	@Override
	public void onReceive(Context context, Intent intent)
	{
		Log.d("lol", "scanned: " + wifiManager.getScanResults().size());
		
		synchronized(scanResults)
		{
			scanResults = wifiManager.getScanResults();	
		}
		
		if(isCalibration && isStarted)
		{
			synchronized(calibrationDatas)
			{
				calibrationDatas.add(new CalibrationData(scanResults, x, y));
				
				Log.d("lol", "calibrationDatas size: " + calibrationDatas.size());
			}
		}
		
		if(callback != null)
			callback.callback();
	}

	@Override
	public void run()
	{
		while(isRunning)
		{
			Log.d("lol","start meas");
			wifiManager.startScan();
			
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
