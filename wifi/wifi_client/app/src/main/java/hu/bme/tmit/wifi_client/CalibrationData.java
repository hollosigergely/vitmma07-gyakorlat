package hu.bme.tmit.wifi_client;

import java.util.List;

import android.net.wifi.ScanResult;


public class CalibrationData
{
	public List<ScanResult> scanResults;
	public float x;
	public float y;
	
	public CalibrationData(List<ScanResult> scanResults, float x, float y)
	{
		super();
		this.scanResults = scanResults;
		this.x = x;
		this.y = y;
	}
}
