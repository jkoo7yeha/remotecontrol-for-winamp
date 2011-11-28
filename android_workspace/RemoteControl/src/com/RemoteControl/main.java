package com.RemoteControl;

import com.RemoteControl.RemoteControlOverview.UpdateTimeTask;
import com.RemoteControl.R;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.nio.channels.IllegalBlockingModeException;
import java.text.DecimalFormat;
import java.util.Timer;

import android.app.Dialog;
import android.app.ProgressDialog;
import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.telephony.TelephonyManager;
import android.text.method.LinkMovementMethod;
import android.util.DisplayMetrics;
import android.view.ViewGroup;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

public class main extends TabActivity implements OnClickListener,
		OnTouchListener {

	private static main activity = null;

	private final static Handler error_class_Handler = new Handler();

	private static TabHost tabHost;

	// //////////////// VIEWS //////////////////////
	private static ImageButton button_previous, button_playPause, button_next;
	private static LinearLayout buttonLayout;

	private static Button button_connect;

	// //////////////// SOCKET ////////////////////

	private static DecimalFormat df = new DecimalFormat("00");

	private static volatile Socket socket = null;
	private static volatile InputStream inputStream;

	private static volatile Handler ConnectionHandler;

	private static ProgressDialog connect_dialog;

	// ///////////// TELEPHONY MANAGER /////////////
	private static IncomingCallListener phoneListener = new IncomingCallListener();
	private static TelephonyManager telephonyManager;

	// ///////////// OTHERS /////////////////

	private static CoverReader coverReader = new CoverReader();

	private static Thread receiveThread;

	private static PlaybackSettings playbackSettings;

	private static PreferenceReader prefReader = new PreferenceReader();

	@Override
	public void onStart() {
		super.onStart();

		prefReader.getPrefs();
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {

		// if not tablet force portrait
		if (Build.VERSION.SDK_INT < 11)
			this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		super.onCreate(savedInstanceState);

		setContentView(R.layout.main);

		// GET POWER MANAGER
		Settings.pm = (PowerManager) getSystemService(Context.POWER_SERVICE);

		// DISPLAY METRICS
		Settings.dm = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(Settings.dm);

		// GET TELEPHONYMANAGER
		setTelephonyManager((TelephonyManager) getSystemService(TELEPHONY_SERVICE));

		// new playbackSettings
		playbackSettings = new PlaybackSettings();

		// initialize variables
		ToastClass.text_error = "ERROR";
		ToastClass.duration_short = Toast.LENGTH_LONG;
		ToastClass.toast_error = Toast.makeText(getApplicationContext(),
				ToastClass.text_error, ToastClass.duration_short);

		playbackSettings.setRepeat_status('0');
		playbackSettings.setShuffle_status('0');

		setConnectionHandler(new Handler());

		setActivity(this);

		Resources res = getResources(); // Resource object to get Drawables
		setTabHost(getTabHost()); // The activity TabHost
		TabHost.TabSpec spec; // Resusable TabSpec for each tab

		Intent intent;

		// OVERVIEW TAB
		intent = new Intent().setClass(this, RemoteControlOverview.class);
		spec = getTabHost()
				.newTabSpec("Overview")
				.setIndicator("Overview",
						res.getDrawable(R.drawable.overview_white))
				.setContent(intent);
		getTabHost().addTab(spec);

		// PLAYLIST TAB
		intent = new Intent().setClass(this, RemoteControlPlaylist.class);
		spec = getTabHost()
				.newTabSpec("Playlist")
				.setIndicator("Playlist",
						res.getDrawable(R.drawable.playlist_white))
				.setContent(intent);
		getTabHost().addTab(spec);

		// SETTINGS TAB
		intent = new Intent().setClass(this, RemoteControlSettings.class);
		spec = getTabHost()
				.newTabSpec("Settings")
				.setIndicator("Settings",
						res.getDrawable(R.drawable.settings_white))
				.setContent(intent);
		getTabHost().addTab(spec);

		// STANDARD OPENED
		getTabHost().setCurrentTab(0);

		// set listeners
		setlisteners();
	}

	private void setlisteners() {
		setButton_connect((Button) findViewById(R.string.connect));
		getButton_connect().setOnClickListener(this);

		buttonLayout = (LinearLayout) findViewById(R.string.buttonLayout);

		button_previous = (ImageButton) findViewById(R.string.previous);
		button_previous.setOnClickListener(this);
		button_previous.setOnTouchListener(this);

		button_playPause = (ImageButton) findViewById(R.string.playPause);
		button_playPause.setOnClickListener(this);
		button_playPause.setOnTouchListener(this);

		button_next = (ImageButton) findViewById(R.string.next);
		button_next.setOnClickListener(this);
		button_next.setOnTouchListener(this);
	}

	private void unbindDrawables(View view) {
		if (view.getBackground() != null) {
			view.getBackground().setCallback(null);
		}

		if (view instanceof ViewGroup) {
			for (int i = 0; i < ((ViewGroup) view).getChildCount(); i++)
				unbindDrawables(((ViewGroup) view).getChildAt(i));

			if (view instanceof AdapterView) {
				for (int i = 0; i < ((AdapterView<?>) view).getChildCount(); i++)
					unbindDrawables(((AdapterView<?>) view).getChildAt(i));
			} else
				((ViewGroup) view).removeAllViews();
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();

		disconnect();

		try {
			RemoteControlOverview.activity.finish();
			RemoteControlPlaylist.activity.finish();
			RemoteControlSettings.activity.finish();
		} catch (Exception e) {
		}

		// RELEASE DISPLAY LOCK
		try {
			Settings.wl.release();
		} catch (Exception e) {
		}

		// unbind drawables and tell garbage collector to work
		unbindDrawables(findViewById(android.R.id.tabhost));
		System.gc();
	}

	static void showHelp() {
		final Dialog dialog = new Dialog(activity);

		dialog.setContentView(R.layout.help_dialog);
		dialog.setTitle("What's next?");

		// activate short links
		TextView t1 = (TextView) dialog.getWindow()
				.findViewById(R.string.help2);
		t1.setMovementMethod(LinkMovementMethod.getInstance());

		TextView t2 = (TextView) dialog.getWindow()
				.findViewById(R.string.help6);
		t2.setMovementMethod(LinkMovementMethod.getInstance());

		dialog.show();

		dialog.getWindow().findViewById(R.string.connectButton)
				.setOnClickListener(new OnClickListener() {

					@Override
					public void onClick(View v) {
						dialog.cancel();

						if (getSocket() == null) {
							setConnect_dialog(ProgressDialog.show(activity, "",
									"Connecting to \n" + Settings.liststatus[0]
											+ ":" + Settings.liststatus[1]
											+ "\n ...", true));

							ConnectThread();
						}
					}
				});
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// SOCKET
	// //////////////////////////////////////////////

	private static int connect() {
		SocketAddress sockaddr = null;
		InetAddress addr = null;

		// check ip address
		try {
			addr = InetAddress.getByName((String) Settings.liststatus[0]);
		} catch (UnknownHostException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.ip_error);

			return 1;
		}

		// check port
		try {
			sockaddr = new InetSocketAddress(addr,
					Integer.parseInt((String) Settings.liststatus[1]));
		} catch (IllegalArgumentException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.port_error);

			return 1;
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		// check socket

		setSocket(new Socket());

		try {
			getSocket().connect(sockaddr,
					Integer.parseInt((String) Settings.liststatus[2]));
		} catch (SocketTimeoutException e) {
			e.printStackTrace();

			try {
				getSocket().close();
			} catch (Exception e1) {
				e1.printStackTrace();
			}

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.connect_timeout);

			setSocket(null);

			return 1;

		} catch (IllegalBlockingModeException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.channel_error);

			setSocket(null);

			return 1;
		} catch (IllegalArgumentException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.endpointer_error);

			setSocket(null);

			return 1;
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler()
					.post(ErrorMessagesClass.unknown_socket_error);

			setSocket(null);

			return 1;
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		try {
			SendClass.outToServer = new DataOutputStream(getSocket()
					.getOutputStream());
			SendClass.start();
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.output_stream_state);

			setSocket(null);

			return 1;
		}

		try {
			setInputStream(getSocket().getInputStream());
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.input_stream_state);

			setSocket(null);

			return 1;
		}

		return 0;
	}

	final static Runnable changeButtonsConnected = new Runnable() {
		public void run() {
			getButton_connect().setVisibility(8);

			buttonLayout.setVisibility(0);

			button_previous.setVisibility(0);
			button_playPause.setVisibility(0);
			button_next.setVisibility(0);
		}
	};

	final static Runnable changeButtonsDisconnected = new Runnable() {
		public void run() {
			RemoteControlOverview.title_display.setText("");

			playbackSettings.setRepeat_status('0');
			playbackSettings.setShuffle_status('0');

			RemoteControlOverview.button_repeat.setChecked(false);
			RemoteControlOverview.button_shuffle.setChecked(false);

			RemoteControlOverview.seekbarProgress.setProgress(0);
			RemoteControlOverview.current_length_seconds_display.setText("00");
			RemoteControlOverview.current_length_minutes_display.setText("00");
			RemoteControlOverview.length_seconds_display.setText("00");
			RemoteControlOverview.length_minutes_display.setText("00");

			RemoteControlOverview.samplerate_display.setText("");
			RemoteControlOverview.bitrate_display.setText("");

			getButton_connect().setVisibility(0);

			buttonLayout.setVisibility(8);

			button_previous.setVisibility(8);
			button_playPause.setVisibility(8);
			button_next.setVisibility(8);
		}
	};

	private static void ConnectThread() {

		Thread t = new Thread() {
			public void run() {

				if (connect() == 0)
					if (synchronize() == 0) {
						getConnectionHandler().post(changeButtonsConnected);

						// SET DISPLAY LOCK

						if (Settings.liststatus[4].equals("Dim"))
							Settings.wl = Settings.pm.newWakeLock(
									PowerManager.SCREEN_DIM_WAKE_LOCK, "Dim");
						else if (Settings.liststatus[4].equals("Always on"))
							Settings.wl = Settings.pm.newWakeLock(
									PowerManager.SCREEN_BRIGHT_WAKE_LOCK,
									"Always on");

						try {
							Settings.wl.acquire();
						} catch (Exception e) {
						}

					}
			}
		};
		t.start();
	}

	private static int synchronize() {

		// ///////////////////////////////// update connect_dialog
		// ///////////////////////////////////////

		RemoteControlOverview.WinampSettingsHandler
				.post(RemoteControlOverview.Synchronizing);

		// ///////////////////////////////// create new coverHashMap
		// ///////////////////////////////////////

		getCoverReader().resetCoverList();

		// ///////////////////////////////// PLAYLISTLENGTH
		// ///////////////////////////////////////

		try {
			Settings.playlistlength = Integer.parseInt(UTF8Reader
					.readLine(getInputStream()));
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		Settings.playlist = new PlaylistElement[Settings.playlistlength];

		// ///////////////////////////////// PLAYLIST
		// ///////////////////////////////////////

		int i, tmpCoverLength;
		CharSequence tmpMessage = null;
		Bitmap tmpCover;

		for (i = 0; i < Settings.playlistlength; i++) {
			try {
				// read cover
				tmpMessage = UTF8Reader.readLine(getInputStream());

				tmpMessage = tmpMessage.subSequence(12, tmpMessage.length());
				tmpCoverLength = Integer.parseInt(tmpMessage.toString());

				tmpCover = getCoverReader().readCover(tmpCoverLength, true);

				// create new playlist element
				Settings.playlist[i] = new PlaylistElement(
						UTF8Reader.readLine(getInputStream()), i, tmpCover);

			} catch (IOException e) {
				e.printStackTrace();

				getConnect_dialog().dismiss();
				getErrorClassHandler().post(ErrorMessagesClass.reader_error);

				getActivity().disconnect();

				return 1;
			}
		}

		// ///////////////////////////////// REPEAT
		// ///////////////////////////////////////

		try {
			playbackSettings.setRepeat_status(UTF8Reader.readLine(
					getInputStream()).charAt(0));
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// ///////////////////////////////// SHUFFLE
		// ///////////////////////////////////////

		try {
			playbackSettings.setShuffle_status(UTF8Reader.readLine(
					getInputStream()).charAt(0));
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// ///////////////////////////////// VOLUME
		// ///////////////////////////////////////

		String volume_string = new String();

		try {
			volume_string = UTF8Reader.readLine(getInputStream());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		try {
			playbackSettings.setVolume(Integer.parseInt(volume_string));
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		// ///////////////////////////////// QUEUE LIST COUNT
		// ///////////////////////////////////////

		int queueCount = 0;

		try {
			queueCount = Integer
					.parseInt(UTF8Reader.readLine(getInputStream()));
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		// ///////////////////////////////// QUEUE LIST ELEMENTS
		// ///////////////////////////////////////

		int j;

		for (j = 0; j < queueCount; j++) {
			try {
				Settings.Queue.add(Integer.parseInt(UTF8Reader
						.readLine(getInputStream())));
			} catch (IOException e) {
				e.printStackTrace();

				getConnect_dialog().dismiss();
				getErrorClassHandler().post(ErrorMessagesClass.reader_error);

				getActivity().disconnect();

				return 1;
			} catch (Exception e) {
				getErrorClassHandler()
						.post(ErrorMessagesClass.conversion_error);
			}
		}

		// ///////////////////////////////// SAMPLERATE
		// ///////////////////////////////////////

		try {
			playbackSettings.setSamplerate(Integer.parseInt(UTF8Reader
					.readLine(getInputStream())));
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		// ///////////////////////////////// BITRATE
		// ///////////////////////////////////////

		try {
			playbackSettings.setBitrate(UTF8Reader.readLine(getInputStream())
					.toCharArray());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// ///////////////////////////////// LENGTH
		// ///////////////////////////////////////

		String length_temp = new String();

		try {
			length_temp = UTF8Reader.readLine(getInputStream());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		try {
			playbackSettings.setLength(Integer.parseInt(length_temp));
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}
		RemoteControlOverview.seekbarProgress.setMax(playbackSettings
				.getLength());

		// ///////////////////////////////// PLAYLIST POSITION
		// ///////////////////////////////////////

		String playlistPosition_temp = new String();

		try {
			playlistPosition_temp = UTF8Reader.readLine(getInputStream());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		try {
			Settings.playlistPosition = Integer.parseInt(playlistPosition_temp);
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		// ///////////////////////////////// TITLE
		// ///////////////////////////////////////

		try {
			playbackSettings.setTitle(UTF8Reader.readLine(getInputStream()));
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// ///////////////////////////////// PLAYBACK POSITION
		// ///////////////////////////////////////

		String position = new String();

		try {
			position = UTF8Reader.readLine(getInputStream());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// ///////////////////////////////// PLAYBACK STATUS
		// ///////////////////////////////////////

		String isPlaying_temp = new String();

		try {
			isPlaying_temp = UTF8Reader.readLine(getInputStream());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		try {
			playbackSettings.setIsPlaying(Integer.parseInt(isPlaying_temp));
		} catch (Exception e) {
			getErrorClassHandler().post(ErrorMessagesClass.conversion_error);
		}

		if (playbackSettings.getIsPlaying() == 1) // PLAY
		{
			try {
				RemoteControlOverview.timertask.cancel();
				RemoteControlOverview.timertask.cancel = true;
				RemoteControlOverview.timer.cancel();
			} catch (Exception e) {
			}

			playbackSettings.setStartTime(System.currentTimeMillis()
					- playbackSettings.getCurrent_millis());

			RemoteControlOverview.timer = new Timer();
			RemoteControlOverview.timertask = new UpdateTimeTask();
			RemoteControlOverview.timer.schedule(
					RemoteControlOverview.timertask, 0, 1000);

			getConnectionHandler().post(pause_runnable);
		} else if (playbackSettings.getIsPlaying() == 3) // PAUSE
		{
			try {
				RemoteControlOverview.timertask.cancel();
				RemoteControlOverview.timertask.cancel = true;
				RemoteControlOverview.timer.cancel();
			} catch (Exception e) {
			}

			playbackSettings.setIsPlaying(3);

			playbackSettings.setStartTime(System.currentTimeMillis()
					- playbackSettings.getCurrent_millis());
		} else if (playbackSettings.getIsPlaying() == 0) // STOP
			getConnectionHandler().post(play_runnable);

		// PROGRESSBAR
		if (!position.contains("-1")) {
			playbackSettings.setCurrent_millis(Long.valueOf(position)
					.longValue());
			playbackSettings.setCurrent_seconds((int) (playbackSettings
					.getCurrent_millis() / 1000));
			playbackSettings.setCurrent_minutes(playbackSettings
					.getCurrent_seconds() / 60);
			playbackSettings.setCurrent_seconds(playbackSettings
					.getCurrent_seconds() % 60);

			playbackSettings.setStartTime(System.currentTimeMillis()
					- playbackSettings.getCurrent_millis());

			RemoteControlOverview.currentTimeHandler.post(UpdateCurrentTime);

			int progress = playbackSettings.getCurrent_seconds()
					+ (playbackSettings.getCurrent_minutes() * 60);
			RemoteControlOverview.seekbarProgress.setProgress(progress);
		}

		// ///////////////////////////////// COVER
		// ///////////////////////////////////////////////////

		CharSequence message;
		// LENGTH
		try {
			message = UTF8Reader.readLine(getInputStream());
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		try {
			message = message.subSequence(12, message.length());
			tmpCoverLength = Integer.parseInt(message.toString());
		} catch (Exception e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		// COVER

		try {
			tmpCover = getCoverReader().readCover(tmpCoverLength, false);
		} catch (IOException e) {
			e.printStackTrace();

			getConnect_dialog().dismiss();
			getErrorClassHandler().post(ErrorMessagesClass.reader_error);

			getActivity().disconnect();

			return 1;
		}

		if (tmpCover != null) {
			RemoteControlOverview.cover = tmpCover;

			RemoteControlOverview.WinampSettingsHandler
					.post(RemoteControlOverview.SetCover);

		} else
			RemoteControlOverview.WinampSettingsHandler
					.post(RemoteControlOverview.SetEmptyCover);

		// start listening for commands
		receiveThread = new Thread(new ReceiveClass());
		receiveThread.start();

		try {
			RemoteControlPlaylist.viewHandler
					.post(RemoteControlPlaylist.initialize);
		} catch (Exception e) {
		}

		RemoteControlOverview.WinampSettingsHandler
				.post(RemoteControlOverview.UpdateUI);
		RemoteControlOverview.WinampSettingsHandler
				.post(RemoteControlOverview.UpdateVolume);

		return 0;

	}

	protected void ReceiveThread() {

		Thread ReceiveThread = new Thread() {
			public void run() {

			}

		};
		ReceiveThread.start();
	}

	final static Runnable stop_runnable = new Runnable() {
		public void run() {
			RemoteControlOverview.seekbarProgress.setProgress(0);
			RemoteControlOverview.current_length_seconds_display.setText("00");
			RemoteControlOverview.current_length_minutes_display.setText("00");
		}
	};

	final static Runnable UpdateCurrentTime = new Runnable() {
		public void run() {
			RemoteControlOverview.current_length_minutes_display.setText(df
					.format(playbackSettings.getCurrent_minutes()));
			RemoteControlOverview.current_length_seconds_display.setText(df
					.format(playbackSettings.getCurrent_seconds()));
		}
	};

	final static Runnable pause_runnable = new Runnable() {
		public void run() {
			button_playPause.setImageResource(R.drawable.pause);
		}
	};

	final static Runnable play_runnable = new Runnable() {
		public void run() {
			button_playPause.setImageResource(R.drawable.play);
		}
	};

	final static Runnable UpdateProgressTimer = new Runnable() {
		public void run() {

			int progress = playbackSettings.getCurrent_seconds()
					+ (playbackSettings.getCurrent_minutes() * 60);

			if (RemoteControlOverview.seekbarProgress.isPressed()) {
				RemoteControlOverview.seekbarProgress
						.setSecondaryProgress(progress);
			} else {
				RemoteControlOverview.seekbarProgress.setProgress(progress);
			}

			if (progress == RemoteControlOverview.seekbarProgress.getMax()) {
				try {
					RemoteControlOverview.timertask.cancel();
					RemoteControlOverview.timertask.cancel = true;
					RemoteControlOverview.timer.cancel();
				} catch (Exception e) {
				}
			}
		}
	};

	int disconnect() {
		try {
			RemoteControlOverview.timertask.cancel();
			RemoteControlOverview.timertask.cancel = true;
			RemoteControlOverview.timer.cancel();
		} catch (Exception e) {
		}

		try {
			SendClass.queueOut.add("destroy");
			SendClass.outToServer = null;

			if (getSocket() != null) {
				// socket.shutdownInput();

				try {
					getSocket().shutdownOutput(); // needed so winamp recognizes
													// disconnect!
					getSocket().shutdownInput();
					getSocket().getInputStream().close();
					getSocket().getOutputStream().close();
					getSocket().close();
				} catch (Exception e) {
				}
				// socket.close();

				setSocket(null);
			}

		} catch (Exception e) {
			e.printStackTrace();

			setInputStream(null);
			SendClass.outToServer = null;
			setSocket(null);

			getConnect_dialog().dismiss();
			// error_class.error_class_Handler.post(error_class.disconnecting_error);
		}

		// CLEAR EVERYTHING
		getConnectionHandler().post(changeButtonsDisconnected);

		RemoteControlOverview.WinampSettingsHandler
				.post(RemoteControlOverview.SetEmptyCover);

		RemoteControlOverview.seekbarVolume.setProgress(0);

		RemoteControlOverview.seekbarProgress.setProgress(0);
		RemoteControlOverview.seekbarProgress.setSecondaryProgress(0);

		Settings.playlist = null;
		Settings.playlistlength = 0;
		Settings.playlistPosition = 0;
		Settings.Queue.clear();

		// clear playlist
		try {
			RemoteControlPlaylist.refreshItems();
		} catch (Exception e) {
		}

		// RELEASE DISPLAY LOCK
		try {
			Settings.wl.release();
		} catch (Exception e) {
		}

		return 0;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// ONCLICK/ONTOUCH
	// /////////////////////////////////////

	@Override
	public void onClick(View v) {

		switch (v.getId()) {
		case R.string.connect:

			if (getSocket() == null) {
				setConnect_dialog(ProgressDialog.show(this, "",
						"Connecting to \n" + Settings.liststatus[0] + ":"
								+ Settings.liststatus[1] + "\n ...", true));

				ConnectThread();
			}

			break;

		case R.string.previous:
			if (getSocket() != null)
				SendClass.queueOut.add("previous");
			break;

		case R.string.playPause:
			if (getSocket() != null) {
				if (playbackSettings.getIsPlaying() == 1) {
					SendClass.queueOut.add("pause");
					getConnectionHandler().post(play_runnable);
				} else {
					SendClass.queueOut.add("play");
					getConnectionHandler().post(pause_runnable);
				}
			}
			break;

		case R.string.next:
			if (getSocket() != null)
				SendClass.queueOut.add("next");
			break;
		}
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {

		switch (v.getId()) {
		case R.string.next:
			if (event.getAction() == MotionEvent.ACTION_DOWN)
				((ImageButton) v).setImageResource(R.drawable.next_pressed);
			else if (event.getAction() == MotionEvent.ACTION_UP)
				((ImageButton) v).setImageResource(R.drawable.next);
			break;

		case R.string.previous:
			if (event.getAction() == MotionEvent.ACTION_DOWN)
				((ImageButton) v).setImageResource(R.drawable.previous_pressed);
			else if (event.getAction() == MotionEvent.ACTION_UP)
				((ImageButton) v).setImageResource(R.drawable.previous);
			break;

		case R.string.playPause:

			if (playbackSettings.getIsPlaying() == 1) {
				if (event.getAction() == MotionEvent.ACTION_DOWN)
					((ImageButton) v)
							.setImageResource(R.drawable.pause_pressed);
				else if (event.getAction() == MotionEvent.ACTION_UP)
					((ImageButton) v).setImageResource(R.drawable.pause);
			} else // paused
			{
				if (event.getAction() == MotionEvent.ACTION_DOWN)
					((ImageButton) v).setImageResource(R.drawable.play_pressed);
				else if (event.getAction() == MotionEvent.ACTION_UP)
					((ImageButton) v).setImageResource(R.drawable.play);
			}
			break;

		}

		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// MENU
	// ////////////////////////////////////////////////

	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		if (getSocket() == null) {
			menu.findItem(R.string.menu_disConnect).setTitle("Connect");

			if (Build.VERSION.SDK_INT >= 9)
				menu.findItem(R.string.menu_disConnect).setIcon(
						R.drawable.connect_white);
			else
				menu.findItem(R.string.menu_disConnect).setIcon(
						R.drawable.connect_black);
		} else {
			menu.findItem(R.string.menu_disConnect).setTitle("Disconnect");

			if (Build.VERSION.SDK_INT >= 9)
				menu.findItem(R.string.menu_disConnect).setIcon(
						R.drawable.disconnect_white);
			else
				menu.findItem(R.string.menu_disConnect).setIcon(
						R.drawable.disconnect_black);
		}

		return super.onPrepareOptionsMenu(menu);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();

		if (Build.VERSION.SDK_INT >= 9)
			inflater.inflate(R.menu.menu_bottom_white, menu);
		else
			inflater.inflate(R.menu.menu_bottom_black, menu);

		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {

		switch (item.getItemId()) {
		case R.string.menu_disConnect:
			if (getSocket() == null) {
				setConnect_dialog(ProgressDialog.show(this, "",
						"Connecting to \n" + Settings.liststatus[0] + ":"
								+ Settings.liststatus[1] + "\n ...", true));

				ConnectThread();
			} else
				disconnect();

			return true;

		case R.string.menu_about:

			Dialog dialog = new Dialog(this);

			dialog.setContentView(R.layout.about_dialog);
			dialog.setTitle("RemoteControl");

			PackageInfo info = null;
			try {
				info = getPackageManager().getPackageInfo(getPackageName(), 0);
			} catch (NameNotFoundException e) {
				e.printStackTrace();
			}

			TextView version = (TextView) dialog
					.findViewById(R.string.infotext1);
			version.setText("Version: " + info.versionName);

			dialog.show();

			dialog.getWindow().findViewById(R.string.donate)
					.setOnClickListener(new OnClickListener() {

						@Override
						public void onClick(View v) {
							String url = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4ZME6LEXD23GW"; // You
																																// could
																																// have
																																// this
																																// at
																																// the
																																// top
																																// of
																																// the
																																// class
																																// as
																																// a
																																// constant,
																																// or
																																// pass
																																// it
																																// in
																																// as
																																// a
																																// method
																																// variable,
																																// if
																																// you
																																// wish
																																// to
																																// send
																																// to
																																// multiple
																																// websites
							Intent i = new Intent(Intent.ACTION_VIEW); // Create
																		// a new
																		// intent
																		// -
																		// stating
																		// you
																		// want
																		// to
																		// 'view
																		// something'
							i.setData(Uri.parse(url)); // Add the url data
														// (allowing android to
														// realise you want to
														// open the browser)
							startActivity(i);

						}
					});

			return true;

		case R.string.menu_help:

			showHelp();

			return true;

		case R.string.menu_donate:

			String url = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4ZME6LEXD23GW";
			Intent i = new Intent(Intent.ACTION_VIEW);
			i.setData(Uri.parse(url));
			startActivity(i);

			return true;

		case R.string.menu_quit:

			finish();

			return true;

		default:
			return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		int action = event.getAction();
		int keyCode = event.getKeyCode();

		switch (keyCode) {
		case KeyEvent.KEYCODE_VOLUME_UP:
			if (action == KeyEvent.ACTION_DOWN) {
				if (RemoteControlOverview.seekbarVolume.getProgress() >= 48)
					RemoteControlOverview.seekbarVolume
							.setProgress(RemoteControlOverview.seekbarVolume
									.getProgress() + 32);
				else
					RemoteControlOverview.seekbarVolume
							.setProgress(RemoteControlOverview.seekbarVolume
									.getProgress() + 16);

				SendClass.queueOut.add("volume_".concat(String
						.valueOf(RemoteControlOverview.seekbarVolume
								.getProgress())));
			}

			return true;

		case KeyEvent.KEYCODE_VOLUME_DOWN:
			if (action == KeyEvent.ACTION_DOWN) {
				if (RemoteControlOverview.seekbarVolume.getProgress() >= 48)
					RemoteControlOverview.seekbarVolume
							.setProgress(RemoteControlOverview.seekbarVolume
									.getProgress() - 32);
				else
					RemoteControlOverview.seekbarVolume
							.setProgress(RemoteControlOverview.seekbarVolume
									.getProgress() - 16);

				SendClass.queueOut.add("volume_".concat(String
						.valueOf(RemoteControlOverview.seekbarVolume
								.getProgress())));
			}
			return true;

		default:
			return super.dispatchKeyEvent(event);
		}
	}

	public static void setInputStream(InputStream input) {
		inputStream = input;
	}

	public static InputStream getInputStream() {
		return inputStream;
	}

	public static Handler getErrorClassHandler() {
		return error_class_Handler;
	}

	public static void setSocket(Socket input) {
		socket = input;
	}

	public static Socket getSocket() {
		return socket;
	}

	public static void setConnect_dialog(ProgressDialog input) {
		connect_dialog = input;
	}

	public static ProgressDialog getConnect_dialog() {
		return connect_dialog;
	}

	public static void setConnectionHandler(Handler connectionHandler) {
		ConnectionHandler = connectionHandler;
	}

	public static Handler getConnectionHandler() {
		return ConnectionHandler;
	}

	public static void setCoverReader(CoverReader input) {
		coverReader = input;
	}

	public static CoverReader getCoverReader() {
		return coverReader;
	}

	public static void setActivity(main input) {
		activity = input;
	}

	public static main getActivity() {
		return activity;
	}

	public static void setTabHost(TabHost input) {
		tabHost = input;
	}

	public static TabHost getActivityTabHost() {
		return tabHost;
	}

	public static void setTelephonyManager(TelephonyManager input) {
		telephonyManager = input;
	}

	public static TelephonyManager getTelephonyManager() {
		return telephonyManager;
	}

	public static void setPhoneListener(IncomingCallListener input) {
		phoneListener = input;
	}

	public static IncomingCallListener getPhoneListener() {
		return phoneListener;
	}

	public static synchronized final PlaybackSettings getPlaybackSettings() {
		return playbackSettings;
	}

	public static synchronized final void setPlaybackSettings(
			PlaybackSettings playbackSettings) {
		main.playbackSettings = playbackSettings;
	}

	public static void setButton_connect(Button button_connect) {
		main.button_connect = button_connect;
	}

	public static Button getButton_connect() {
		return button_connect;
	}
}
