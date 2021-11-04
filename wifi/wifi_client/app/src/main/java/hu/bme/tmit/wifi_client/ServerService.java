package hu.bme.tmit.wifi_client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.IBinder;
import android.util.Log;


public class ServerService extends Service implements Runnable
{
	public static boolean isRunning = false;
	private Thread backgroundThread = null;
	private Thread backgroundThread2 = null;
	private ServerSocket serverSocket = null;
	private WifiManager wifiManager = null;
	private MyBroadcastReceiver bcr = null;
	
	
	@Override
	public IBinder onBind(Intent intent)
	{
		return null;
	}

	@Override
	public void onCreate()
	{
		super.onCreate();
		
		isRunning = true;
		
		Log.d("lol", "ServerService started");
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
	{
		if(backgroundThread != null && backgroundThread.isAlive())
		{
			release();
		}
		
		if(backgroundThread == null || !backgroundThread.isAlive())
		{	
			isRunning = true;
			backgroundThread = new Thread(this);
			backgroundThread.start();
			
			wifiManager = (WifiManager)this.getSystemService(Context.WIFI_SERVICE);			
			bcr = new MyBroadcastReceiver(wifiManager, false, null);
			this.registerReceiver(bcr, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
			
			backgroundThread2 = new Thread(bcr);
			backgroundThread2.start();
			
			Log.d("lol", "backgroundThread started");
		}
		return super.onStartCommand(intent, flags, startId);
	}
	
	@Override
	public void onDestroy()
	{
		super.onDestroy();
		
		isRunning = false;
		release();
		
		Log.d("lol", "ServerService stopped");
	}
	
	void release()
	{
		if(backgroundThread != null && backgroundThread.isAlive())
		{
			try {
				isRunning = false;
				bcr.isRunning = false;
				if(serverSocket != null)
					serverSocket.close();
				
				this.unregisterReceiver(bcr);
				
				backgroundThread2.interrupt();
				backgroundThread2.join();
				backgroundThread2 = null;
				
				backgroundThread.interrupt();
				backgroundThread.join();
				backgroundThread = null;
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
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
	
	@Override
	public void run()
	{
    	Log.d("lol", "Running started");
    	
    	try {
	    	serverSocket = new ServerSocket(8080);
			serverSocket.setReuseAddress(true);
	    	
	    	while(isRunning)
	    	{
	    		try {
	    			Socket socket = serverSocket.accept();

	    			Log.d("lol", "Socket accepted");
	    			
	    			BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	    			PrintStream printStream = new PrintStream(socket.getOutputStream(), true);
	    			
	    			String line;
	    			while ((line = in.readLine()) != null)
	    			{
	    				if (line.length() == 0)
	    					break;
	    				//Log.d("lol", line);
	    			}
	    			
	    			printStream.println("HTTP/1.1 200 OK");
	    			printStream.println("Content-Type: text/html");
	    			printStream.println();
	    			
	    			List<ScanResult> scanResults = bcr.getScanResults();
	    			synchronized(scanResults)
	    			{
	    				for(String mac : MainActivity.APS)
						{
							printStream.print(getDbmByMac(scanResults, mac)+" ");
						}
	    			}
	    			
	    			printStream.close();
	    			in.close();
	    			socket.close();
	    			
	    			Log.d("lol", "Socket closed");
	    		}
	    		catch (IOException e)
	    		{
	    			e.printStackTrace();
	    		}
	    	}
	    	
	    	if(!serverSocket.isClosed())
	    		serverSocket.close();
    	}	
		catch (IOException e)
		{
			e.printStackTrace();
		}
    	
    	Log.d("lol", "Running ended");
		
	}

}
