package com.linuxlabs.remotecamera;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.DisplayMetrics;
import android.view.View;

public class CameraGroupView extends MyScrollView
{
	public static int cameraNumber = 9;

	public CameraGroupView(Context context)
	{
		super(context);
		addCameraList();
		this.setMaxMove(480);
	}
	
	@Override protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        
        Paint paint = new Paint();
        
		paint.setTextSize(30);
		paint.setColor(Color.WHITE);
		canvas.drawText("welcome", 10, 800, paint);
		
		
        postInvalidate();
        
    }


	public void addCamera(CameraButton monitor)
	{
		addView(monitor);
	}

	public void removeCamera(CameraButton monitor)
	{
		removeView(monitor);
	}

	public void addCameraList()
	{
		for (int i = 0; i < cameraNumber; i++)
		{
			final CameraButton camera = new CameraButton(this.getContext(),i, "监控" + (i + 1),R.drawable.camerasmall);
			addView(camera);
		}
	}

	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b)
	{
		// TODO Auto-generated method stub

		DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
		int screenWidth = displayMetrics.widthPixels;
		//int screenHeight = displayMetrics.heightPixels;
		int childTop = 30;
		int childLeft = 20;
		int cameraPicWidth = screenWidth / 5;
		int cameraPicHeight = 2 * cameraPicWidth;
		int space = 20;
		int column = 0;

		final int count = getChildCount();
		for (int i = 0; i < count; i++)
		{
			final View child = getChildAt(i);
			if (child.getVisibility() != View.GONE)
			{
				child.setVisibility(View.VISIBLE);
				//child.measure(r - l, b - t);
				child.layout(childLeft + space, childTop, childLeft + cameraPicWidth + space, childTop + cameraPicHeight + space);
				column++;
				if (childLeft < screenWidth - 2 * (space + cameraPicWidth))
				{
					childLeft += cameraPicWidth + space;
				} else
				{
					column = 0;
					childLeft = 20;
					childTop += cameraPicHeight;
				}
			}
		}
	}

}
