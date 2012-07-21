package com.linuxlabs.remotecamera;

import java.nio.ByteBuffer;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Bitmap.Config;
import android.graphics.drawable.BitmapDrawable;
import android.util.DisplayMetrics;
import android.view.View;


class VideoView extends View implements Runnable
{
	//控制命令
	//控制命令
	public static int OPEN_CAMERA = 1;
	public static int PLAY_VIDEO = 2;
	public static int STOP_VIDEO = 3;
	public static int CLOSE_VIDEO = 4;
	
	private static native void connectTo(String ip);
	private static native void sendCommand(int cmd);
	private static native void initDecoder();
	private static native void displayVideo(byte[] out);
	private static native void closeSocket();

	private int width = 640;
	private int height = 480;
    private byte [] mPixel = new byte[width*height*2];
    
    ByteBuffer buffer = ByteBuffer.wrap( mPixel );
	Bitmap VideoBit = Bitmap.createBitmap(width, height, Config.RGB_565);
	
	private Thread thread;
	static
	{
		System.loadLibrary("ffmpeg");
		System.loadLibrary("native");
	}

	public VideoView(Context context)
	{
		super(context);
       	int i = mPixel.length;
    	
        for(i=0; i<mPixel.length; i++)
        {
        	mPixel[i]=(byte)0x40;
        }

        thread = new Thread(this);
        
	}
	
    public void PlayVideo()
    {
    	thread.start();
    }
    
    public void closeVideo()
    {
    	sendCommand(STOP_VIDEO);
    	closeSocket();
    	
    }
    
	@Override protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        
		DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
		int screenWith = displayMetrics.widthPixels;
    
        VideoBit.copyPixelsFromBuffer(buffer);//makeBuffer(data565, N));
        canvas.drawBitmap(VideoBit, 0, 0, null); 
        
        Paint paint = new Paint();
        
		paint.setTextSize(30);
		paint.setColor(Color.WHITE);
		
		Resources res=getResources();
		BitmapDrawable logo=(BitmapDrawable)res.getDrawable(R.drawable.logo);
		Bitmap bmp=logo.getBitmap();
		canvas.drawBitmap(bmp,5,2, paint);
		canvas.drawText("CCAV现场直播", 40, 30, paint);
		
		paint.setStrokeWidth(3);
		paint.setColor(Color.GREEN);
		
		//left border
		canvas.drawLine(0, 0, 0, 480, paint);
		//top border
		canvas.drawLine(0, 0, screenWith, 0, paint);
		//right border
		canvas.drawLine(screenWith, 0, screenWith, 480, paint);
		//bottom border
		canvas.drawLine(0, 480, screenWith, 480, paint);
		
		
        postInvalidate();
        
    }

	public void run()
	{
		// TODO Auto-generated method stub
		connectTo("192.168.1.18");
		initDecoder();
		sendCommand(PLAY_VIDEO);
		displayVideo(mPixel);
	}
}