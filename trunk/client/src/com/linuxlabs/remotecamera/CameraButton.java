package com.linuxlabs.remotecamera;


import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

public class CameraButton extends ViewGroup
{
	
	public ImageView cameraImage;
	public TextView cameraNameLabel;


	public CameraButton(Context context,int id,String name,int imageId)
	{
		super(context);

		cameraImage = new ImageView(context);
		cameraImage.setImageResource(imageId);
		addView(cameraImage);

		cameraNameLabel = new TextView(context);
		cameraNameLabel.setText(name);
		cameraNameLabel.setTextColor(Color.YELLOW);
		cameraNameLabel.setGravity(Gravity.CENTER);
		cameraNameLabel.setTextSize(16);
		
		addView(cameraNameLabel);
	}
	
	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b)
	{
		// TODO Auto-generated method stub

		int width = 64;
		int height = 64;
		
		final View monitorImage = getChildAt(0);
		monitorImage.layout(0, 0, width, height);

		final View monitorName = getChildAt(1);
		monitorName.layout(0, width, width, 2 * width);
	}
}
