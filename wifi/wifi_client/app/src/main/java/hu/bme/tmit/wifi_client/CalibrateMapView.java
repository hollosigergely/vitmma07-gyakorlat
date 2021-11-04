package hu.bme.tmit.wifi_client;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;


public class CalibrateMapView extends View
{
	private float x = 0;
	private float y = 0;
	private boolean isStarted = false;
	private int calibDataNum = 0;
	
	public CalibrateMapView(Context context, AttributeSet attrs)
	{
		super(context, attrs);
	}
	
	public void setX(float x)
	{
		this.x = x;
	}

	public void setY(float y)
	{
		this.y = y;
	}
	
	public void setStarted(boolean isStarted)
	{
		this.isStarted = isStarted;
	}

	public void setCalibDataNum(int calibDataNum)
	{
		this.calibDataNum = calibDataNum;
	}

	
	@Override
	protected void onDraw(Canvas canvas)
	{
		super.onDraw(canvas);
		
		Paint p = new Paint();
		p.setAntiAlias(true);
		p.setColor(Color.WHITE);
		p.setStyle(Paint.Style.FILL);
		canvas.drawRect(0,0,this.getWidth(),this.getHeight(), p);

		p.setColor(Color.BLACK);
		p.setStyle(Paint.Style.STROKE);
		canvas.drawRect(0,0,this.getWidth(),this.getHeight(), p);
		
		int stepX = (int)Math.floor(getWidth() / 10.0);
		int stepY = (int)Math.floor(getHeight() / 10.0);
		
		for(int i=stepX; i < getWidth()-stepX; i+=stepX)
		{
			canvas.drawLine(i, 0, i, getHeight(), p);
		}
		
		for(int i=stepY; i < getHeight()-stepY; i+=stepY)
		{
			canvas.drawLine(0, i, getWidth(), i, p);
		}
		
		int size = this.getWidth() / 20;
		Paint p2 = new Paint();
		p2.setAntiAlias(true);
		if(isStarted)
			p2.setColor(Color.GREEN);
		else
			p2.setColor(Color.RED);
	
		p2.setStyle(Paint.Style.FILL);
		canvas.drawCircle(x, y, size, p2);
		
		Paint p3 = new Paint(); 
		p3.setColor(Color.WHITE); 
		p3.setStyle(Paint.Style.FILL); 

		p3.setColor(Color.WHITE); 
		p3.setTextSize(size); 
		p3.setTextAlign(Align.CENTER);
		String text = Integer.toString(calibDataNum);
		Rect textBounds = new Rect();
		p3.getTextBounds(text, 0, text.length(), textBounds);
		canvas.drawText(Integer.toString(calibDataNum), x, y-textBounds.exactCenterY(), p3); 
	}
	
}
