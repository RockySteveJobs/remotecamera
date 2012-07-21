/*
 * Copyright 2011 - Churn Labs, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.linuxlabs.remotecamera;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

public class RemoteCamera extends Activity
{

	VideoAndCamerasGroupView group;
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		group = new VideoAndCamerasGroupView(this);
		setContentView(group);
	}

	// Menu item Ids
	public static final int PLAY_ID = Menu.FIRST;
	public static final int EXIT_ID = Menu.FIRST + 1;

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		super.onCreateOptionsMenu(menu);

		menu.add(0, PLAY_ID, 0, "play");
		menu.add(0, EXIT_ID, 1, "stop");

		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
		case PLAY_ID:
		{
			group.videoView.PlayVideo();
			return true;
		}
		case EXIT_ID:
		{
			group.videoView.closeVideo();
			finish();
			return true;
		}
		}
		return super.onOptionsItemSelected(item);
	}
}