package com.RemoteControl;

import java.util.LinkedList;
import java.util.List;

import android.os.PowerManager;
import android.util.DisplayMetrics;

public class Settings {

	// ///////////// PLAYBACK /////////////
	static String[] listoptions = { "Server IP address", "Server Port",
			"Timeout [ms]", "Auto Login", "Display behavior",
			"Pause playback on incoming call" };
	static Object[] liststatus = { "", "", "", false, "", true };

	static volatile int playlistlength = 0;
	static volatile int playlistPosition = 0;

	static volatile List<Object> Queue = new LinkedList<Object>();

	// ///////////// PLAYLIST ELEMENTS /////////////////
	static volatile PlaylistElement[] playlist = null;

	// ///////////// DISPLAY METRICS /////////////
	static volatile DisplayMetrics dm;

	// ///////////// DISPLAY LOCK /////////////
	static volatile PowerManager pm;
	static volatile PowerManager.WakeLock wl;
}
