package com.RemoteControl;

import com.RemoteControl.R;

import java.io.UnsupportedEncodingException;
import java.text.DecimalFormat;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.ProgressDialog;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.text.Html;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

public class RemoteControlOverview extends Activity implements
		SeekBar.OnSeekBarChangeListener, OnClickListener, OnTouchListener {

	static class UpdateTimeTask extends TimerTask {
		public boolean cancel = false;

		public void run() {
			if (cancel == true)
				cancel();
			else {
				main.getPlaybackSettings().setCurrent_millis(
						System.currentTimeMillis()
								- main.getPlaybackSettings().getStartTime());
				main.getPlaybackSettings()
						.setCurrent_seconds(
								(int) (main.getPlaybackSettings()
										.getCurrent_millis() / 1000));
				main.getPlaybackSettings().setCurrent_minutes(
						main.getPlaybackSettings().getCurrent_seconds() / 60);
				main.getPlaybackSettings().setCurrent_seconds(
						main.getPlaybackSettings().getCurrent_seconds() % 60);

				currentTimeHandler.post(main.UpdateCurrentTime);

				currentTimeHandler.post(main.UpdateProgressTimer);
			}
		}
	}

	static RemoteControlOverview activity = null;

	final static Handler error_class_Handler = new Handler();

	static Bitmap cover;

	static ImageView coverView;

	static TextView title_display, length_seconds_display,
			length_minutes_display, current_length_seconds_display,
			current_length_minutes_display, bitrate_display,
			samplerate_display;
	static SeekBar seekbarProgress, seekbarVolume;
	static LinearLayout SeekBarProgressBackground;

	static volatile Timer timer;
	static volatile UpdateTimeTask timertask = new UpdateTimeTask();

	volatile static Handler currentTimeHandler = new Handler();

	static ImageButton button_mute;
	static ToggleButton button_shuffle, button_repeat;

	private volatile Handler DialogHandler = new Handler();

	volatile static Handler WinampSettingsHandler = new Handler();

	static DecimalFormat df = new DecimalFormat("00");

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.overview);

		activity = this;

		// find output views
		title_display = (TextView) findViewById(R.string.title_display);

		length_seconds_display = (TextView) findViewById(R.string.length_seconds);
		length_minutes_display = (TextView) findViewById(R.string.length_minutes);

		current_length_seconds_display = (TextView) findViewById(R.string.current_length_seconds);
		current_length_minutes_display = (TextView) findViewById(R.string.current_length_minutes);

		seekbarProgress = (SeekBar) findViewById(R.string.SeekBarProgress);
		seekbarVolume = (SeekBar) findViewById(R.string.SeekBarVolume);

		samplerate_display = (TextView) findViewById(R.string.samplerate_display);
		bitrate_display = (TextView) findViewById(R.string.bitrate_display);

		coverView = (ImageView) findViewById(R.string.cover);

		if (Settings.dm.densityDpi == DisplayMetrics.DENSITY_MEDIUM
				&& Settings.dm.widthPixels < 400) // mdpi, small screen
		{
			LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
					200, 125);

			coverView.setLayoutParams(layoutParams);

			coverView.setImageResource(R.drawable.cover_small);
		}

		// set listeners
		setlisteners(this);
	}

	private void setlisteners(RemoteControlOverview winampControlClient) {
		button_shuffle = (ToggleButton) findViewById(R.string.shuffle);
		button_shuffle.setOnClickListener(winampControlClient);
		button_shuffle.setOnTouchListener(winampControlClient);

		button_repeat = (ToggleButton) findViewById(R.string.repeat);
		button_repeat.setOnClickListener(winampControlClient);
		button_repeat.setOnTouchListener(winampControlClient);

		button_mute = (ImageButton) findViewById(R.string.mute);
		button_mute.setOnClickListener(winampControlClient);
		button_mute.setOnTouchListener(winampControlClient);

		seekbarVolume.setOnSeekBarChangeListener(winampControlClient);
		seekbarProgress.setOnSeekBarChangeListener(winampControlClient);

	}

	final static Runnable SetEmptyCover = new Runnable() {
		public void run() {

			if (Settings.dm.widthPixels > 400) {
				LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
						400, 300);

				coverView.setLayoutParams(layoutParams);
				coverView.setImageResource(R.drawable.cover_big);
			} else {
				LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
						200, 125);

				coverView.setLayoutParams(layoutParams);
				coverView.setImageResource(R.drawable.cover_small);
			}

		}
	};

	final static Runnable SetCover = new Runnable() {
		public void run() {
			int width, height;

			coverView.setImageBitmap(null);
			LinearLayout.LayoutParams layoutParams;

			if (Settings.dm.widthPixels > 400) { // hdpi, all screen sizes
				width = 400;
				height = 300;
			} else { // mdpi, normal screen
				width = 200;
				height = 125;
			}

			if ((float) (cover.getWidth() / cover.getHeight()) > (float) (width / height)) {
				int layoutwidth = width;
				int layoutheight = (int) (((float) cover.getHeight() / cover
						.getWidth()) * width);

				layoutParams = new LinearLayout.LayoutParams(layoutwidth,
						layoutheight);
				coverView.setLayoutParams(layoutParams);
			}
			if ((float) (cover.getWidth() / cover.getHeight()) < (float) (width / height)) {
				int layoutwidth = (int) (((float) cover.getWidth() / cover
						.getHeight()) * height);
				int layoutheight = height;

				layoutParams = new LinearLayout.LayoutParams(layoutwidth,
						layoutheight);
				coverView.setLayoutParams(layoutParams);
			} else {
				layoutParams = new LinearLayout.LayoutParams(width, height);
				coverView.setLayoutParams(layoutParams);
			}

			coverView.setImageBitmap(cover);
		}
	};

	final static Runnable UpdateDisplays = new Runnable() {
		public void run() {
			String displayedTitle = new String("");
			try {
				displayedTitle = new String(main.getPlaybackSettings()
						.getPlaybackTitle().getBytes(),
						System.getProperty("file.encoding"));
			} catch (UnsupportedEncodingException e) {
				error_class_Handler
						.post(ErrorMessagesClass.unsupportedEncodingError);
				e.printStackTrace();
			}

			title_display.setText(Html.fromHtml(displayedTitle).toString());

			length_minutes_display.setText(df.format(main.getPlaybackSettings()
					.getLength() / 60));
			length_seconds_display.setText(df.format(main.getPlaybackSettings()
					.getLength() % 60));

			seekbarProgress.setMax(main.getPlaybackSettings().getLength());

			bitrate_display.setText(String.valueOf(main.getPlaybackSettings()
					.getBitrate()) + " kbit/s");
			samplerate_display.setText(String.valueOf(main
					.getPlaybackSettings().getSamplerate()) + " Hz");

		}
	};

	final static Runnable UpdateShuffle = new Runnable() {
		public void run() {

			if (main.getPlaybackSettings().getShuffle_status() == '1')
				button_shuffle.setChecked(true);
			else
				button_shuffle.setChecked(false);
		}
	};

	final static Runnable UpdateRepeat = new Runnable() {
		public void run() {

			if (main.getPlaybackSettings().getRepeat_status() == '1')
				button_repeat.setChecked(true);
			else
				button_repeat.setChecked(false);
		}
	};

	final static Runnable UpdateVolume = new Runnable() {
		public void run() {

			seekbarVolume.setProgress(main.getPlaybackSettings().getVolume());

			if (main.getPlaybackSettings().getVolume() == 0)
				button_mute.setImageResource(R.drawable.mute_on);
			else
				button_mute.setImageResource(R.drawable.mute_off);
		}
	};

	final static Runnable UpdateProgress = new Runnable() {
		public void run() {

			main.getPlaybackSettings().setStartTime(
					System.currentTimeMillis()
							- main.getPlaybackSettings().getPlaybackProgress());

			main.getPlaybackSettings().setCurrent_millis(
					main.getPlaybackSettings().getPlaybackProgress());
			main.getPlaybackSettings()
					.setCurrent_seconds(
							(int) (main.getPlaybackSettings()
									.getCurrent_millis() / 1000));
			main.getPlaybackSettings().setCurrent_minutes(
					main.getPlaybackSettings().getCurrent_seconds() / 60);
			main.getPlaybackSettings().setCurrent_seconds(
					main.getPlaybackSettings().getCurrent_seconds() % 60);

			currentTimeHandler.post(main.UpdateCurrentTime);

			int progress = main.getPlaybackSettings().getCurrent_seconds()
					+ (main.getPlaybackSettings().getCurrent_minutes() * 60);
			seekbarProgress.setProgress(progress);
		}
	};

	final static Runnable UpdateUI = new Runnable() {
		public void run() {
			if (main.getPlaybackSettings().getRepeat_status() == '1')
				button_repeat.setChecked(true);

			if (main.getPlaybackSettings().getShuffle_status() == '1')
				button_shuffle.setChecked(true);

			main.getConnectionHandler().post(UpdateDisplays);

			main.getConnect_dialog().dismiss();
		}
	};

	final static Runnable Synchronizing = new Runnable() {
		public void run() {
			main.getConnect_dialog().setMessage("Synchronizing...");
		}
	};

	final Runnable showConnectDialog = new Runnable() {
		public void run() {
			main.setConnect_dialog(ProgressDialog.show(
					RemoteControlOverview.this, "", "Connecting...", true));
		}
	};

	protected void showConnectDialogThread() {
		Thread t = new Thread() {
			public void run() {
				DialogHandler.post(showConnectDialog);
			}
		};
		t.start();
	}

	final Runnable hideConnectDialog = new Runnable() {
		public void run() {
			main.getConnect_dialog().dismiss();
		}
	};

	protected void hideConnectDialogThread() {
		Thread t = new Thread() {
			public void run() {
				DialogHandler.post(hideConnectDialog);
			}
		};
		t.start();
	}

	@Override
	public void onClick(View v) {

		if (main.getSocket() != null) {
			switch (v.getId()) {

			case R.string.shuffle:
				SendClass.queueOut.add("shuffle");

				// change shuffle display
				/*
				 * if (shuffle_status == '0') { shuffle_status = '1';
				 * button_shuffle.setImageResource(R.drawable.shuffle_active); }
				 * else { shuffle_status = '0';
				 * button_shuffle.setImageResource(R.drawable.shuffle); }
				 */

				break;

			case R.string.repeat:
				SendClass.queueOut.add("repeat");

				// change shuffle display
				/*
				 * if (repeat_status == '0') { repeat_status = '1';
				 * button_repeat.setImageResource(R.drawable.repeat_active); }
				 * else { repeat_status = '0';
				 * button_repeat.setImageResource(R.drawable.repeat); }
				 */

				break;

			case R.string.mute:
				SendClass.queueOut.add("mute");

				/*
				 * if (volume != 0) { volume_last = volume; volume = 0;
				 * 
				 * seekbar.setProgress(0); } else { volume = volume_last;
				 * 
				 * seekbar.setProgress(volume_last); }
				 */

				break;

			}
		}

	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {

		switch (v.getId()) {
		case R.string.mute:

			if (main.getSocket() != null) {
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
					if (main.getPlaybackSettings().getVolume() != 0)
						((ImageButton) v)
								.setImageResource(R.drawable.mute_off_pressed);
					else
						((ImageButton) v)
								.setImageResource(R.drawable.mute_on_pressed);
				} else if (event.getAction() == MotionEvent.ACTION_UP) {
					if (main.getPlaybackSettings().getVolume() != 0)
						((ImageButton) v).setImageResource(R.drawable.mute_off);
					else
						((ImageButton) v).setImageResource(R.drawable.mute_on);
				}
			} else {
				if (event.getAction() == MotionEvent.ACTION_DOWN)
					((ImageButton) v)
							.setImageResource(R.drawable.mute_off_pressed);
				else if (event.getAction() == MotionEvent.ACTION_UP)
					((ImageButton) v).setImageResource(R.drawable.mute_off);
			}

			break;

		}

		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// SEEKBAR
	// /////////////////////////////////////////////

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {

		/*
		 * if (seekBar.getId() == R.string.SeekBarVolume) {
		 * send_command.queueOut
		 * .add("volume_".concat(String.valueOf(progress))); }
		 * 
		 * 
		 * else if (seekBar.getId() == R.string.SeekBarProgress)
		 * 
		 * { if (fromUser == true) { startTime = System.currentTimeMillis() -
		 * progress*1000;
		 * send_command.queueOut.add("progress_".concat(String.valueOf
		 * (progress*1000))); } }
		 */
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar) {

		if (seekBar.getId() == R.string.SeekBarProgress) {
			if (main.getPlaybackSettings().getIsPlaying() != 0)
				seekbarProgress.setSecondaryProgress(seekbarProgress
						.getProgress());
		}

	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar) {
		/* //////// RUCKELFREI ///////////// */

		if (seekBar.getId() == R.string.SeekBarProgress) {
			if (main.getPlaybackSettings().getIsPlaying() == 1) {
				main.getPlaybackSettings().setPlaybackProgress(
						seekBar.getProgress() * 1000);
				// if(progress >= seekBar.getProgress()-1)
				// progress -= 2000;

				main.getPlaybackSettings().setStartTime(
						System.currentTimeMillis()
								- main.getPlaybackSettings()
										.getPlaybackProgress());
				SendClass.queueOut.add("progress_".concat(String.valueOf(main
						.getPlaybackSettings().getPlaybackProgress())));

				seekbarProgress.setSecondaryProgress(0);
			} else if (main.getPlaybackSettings().getIsPlaying() == 3) {
				seekbarProgress.setSecondaryProgress(0);

				main.getPlaybackSettings().setCurrent_seconds(
						seekbarProgress.getProgress());
				// if(current_seconds >= seekBar.getProgress()-1)
				// current_seconds -= 2;

				main.getPlaybackSettings()
						.setCurrent_minutes(
								(int) main.getPlaybackSettings()
										.getCurrent_seconds() / 60);
				main.getPlaybackSettings().setCurrent_millis(0);

				currentTimeHandler.post(main.UpdateCurrentTime);

				main.getPlaybackSettings().setPlaybackProgress(
						seekBar.getProgress() * 1000);
				main.getPlaybackSettings().setStartTime(
						System.currentTimeMillis()
								- main.getPlaybackSettings()
										.getPlaybackProgress());
				SendClass.queueOut.add("progress_".concat(String.valueOf(main
						.getPlaybackSettings().getPlaybackProgress())));
			}
		} else if (seekBar.getId() == R.string.SeekBarVolume) {
			SendClass.queueOut.add("volume_".concat(String.valueOf(seekBar
					.getProgress())));
		}

	}

}