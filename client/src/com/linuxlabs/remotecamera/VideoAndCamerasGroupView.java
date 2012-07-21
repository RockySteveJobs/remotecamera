package com.linuxlabs.remotecamera;

import android.content.Context;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;

public class VideoAndCamerasGroupView extends ViewGroup
{

	public VideoView videoView;
	public  CameraGroupView cameraGroupView;

	public VideoAndCamerasGroupView(Context context)
	{
		// TODO Auto-generated constructor stub
		super(context);

		videoView= new VideoView(context);
		addView(videoView);

		cameraGroupView = new CameraGroupView(context);
		addView(cameraGroupView);
	}

	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b)
	{
		DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
		int screenWith = displayMetrics.widthPixels;
		int screenHeight = displayMetrics.heightPixels;

		// TODO Auto-generated method stub
		final View videoView = getChildAt(0);
		videoView.layout(0, 0,screenWith, 480);

		final View cameraGroupView = getChildAt(1);
		cameraGroupView.layout(0, 480,screenWith,screenHeight);

	}
}
