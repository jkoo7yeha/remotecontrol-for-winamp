package com.RemoteControl;

import java.io.UnsupportedEncodingException;

import com.RemoteControl.R;

import android.app.Dialog;
import android.app.ListActivity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.Html;
import android.text.TextWatcher;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.inputmethod.InputMethodManager;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.TabHost.OnTabChangeListener;

public class RemoteControlPlaylist extends ListActivity {

	static Handler error_class_Handler;

	static RemoteControlPlaylist activity = null;

	static EfficientAdapter adapter = null;

	static Handler viewHandler;

	static boolean touchmode = false;

	private Handler touchModeHandler = new Handler();

	static EditText filterText = null;

	ListView listView = null;

	// DIALOG
	static Dialog trackDialog;
	static Bitmap cover;
	static ImageView coverView;
	static TextView titleView, artistView, albumView, yearView, trackView,
			genreView, samplerateView, bitrateView, lengthView, commentView;
	static String title, artist, album, year, track, genre, samplerate,
			bitrate, length, comment;

	static class EfficientAdapter extends ArrayAdapter<PlaylistElement> {

		public EfficientAdapter(Context context, int textViewResourceId,
				int playlistTitle, PlaylistElement[] objects) {
			super(context, textViewResourceId, playlistTitle, objects);

			mInflater = LayoutInflater.from(context);
		}

		private LayoutInflater mInflater;

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			ViewHolder holder;

			if (convertView == null) {
				holder = new ViewHolder();

				convertView = mInflater.inflate(R.layout.listview, null);

				holder.layout = (LinearLayout) convertView
						.findViewById(R.string.playlist_linearlayout);
				holder.text = (TextView) convertView
						.findViewById(R.string.playlist_title);
				holder.playImage = (ImageView) convertView
						.findViewById(R.string.playlist_play);
				holder.coverImage = (ImageView) convertView
						.findViewById(R.string.playlist_cover);
				holder.queue = (TextView) convertView
						.findViewById(R.string.playlist_queue);

				holder.origPosition = position;

				convertView.setTag(holder);
			} else {
				holder = (ViewHolder) convertView.getTag();
			}

			try {
				PlaylistElement item = getItem(position);

				String displayedTitle = new String("");
				try {
					displayedTitle = new String(item.title.getBytes(),
							System.getProperty("file.encoding"));
				} catch (UnsupportedEncodingException e) {
					error_class_Handler
							.post(ErrorMessagesClass.unsupportedEncodingError);
					e.printStackTrace();
				}

				// fill playlist element with content
				holder.text.setText(String.valueOf(item.position + 1) + ". "
						+ Html.fromHtml(displayedTitle).toString());
				holder.origPosition = item.position;

				// set cover if not null
				if (item.cover == null)
					holder.coverImage.setImageResource(R.drawable.cover_square);
				else
					holder.coverImage.setImageBitmap(item.cover);

				// set queue number
				if (Settings.Queue.contains(holder.origPosition))
					holder.queue.setText("["
							+ String.valueOf(Settings.Queue
									.indexOf(holder.origPosition) + 1) + "]");
				else
					holder.queue.setText("");

				// highlight currently playing file
				if (holder.origPosition == Settings.playlistPosition) {
					holder.layout.setBackgroundColor(Color.rgb(200, 200, 200));
					holder.playImage.setImageResource(R.drawable.playlist_play);
				} else {
					holder.layout.setBackgroundColor(Color.rgb(224, 224, 224));
					holder.playImage.setImageResource(0);
				}

			} catch (Exception e) {
				error_class_Handler.post(ErrorMessagesClass.conversion_error);
			}

			return convertView;
		}

		static class ViewHolder {
			LinearLayout layout;
			TextView text, queue;
			int origPosition;
			ImageView playImage, coverImage;
		}

	}

	private static TextWatcher filterTextWatcher = new TextWatcher() {

		public void afterTextChanged(Editable s) {
		}

		public void beforeTextChanged(CharSequence s, int start, int count,
				int after) {
		}

		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			adapter.getFilter().filter(s);
		}

	};

	static Runnable initialize = new Runnable() {
		public void run() {

			adapter = new EfficientAdapter(RemoteControlPlaylist.activity,
					android.R.layout.simple_list_item_1,
					R.string.playlist_title, Settings.playlist);
			RemoteControlPlaylist.activity.setListAdapter(adapter);

			try {
				filterText.addTextChangedListener(filterTextWatcher);
			} catch (Exception e) {
			}

			RemoteControlPlaylist.activity
					.registerForContextMenu(RemoteControlPlaylist.activity
							.getListView());

			try {
				RemoteControlPlaylist.refreshItems();
			} catch (NullPointerException e1) {
			} // playlist not yet loaded

		}
	};

	@Override
	public void onResume() {
		super.onResume();

		// REFRESH PLAYLIST
		if (getListAdapter() == null && Settings.playlist != null)
			viewHandler.post(initialize);

		if (Settings.playlistPosition == 0)
			RemoteControlPlaylist.activity.getListView().setSelection(0);
		else
			RemoteControlPlaylist.activity.getListView().setSelection(
					Settings.playlistPosition - 1);
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.playlist);

		// initialize trackDialog
		trackDialog = new Dialog(this);
		trackDialog.setCancelable(true);
		trackDialog.setContentView(R.layout.track_info_dialog);
		trackDialog.setTitle("Track info");
		coverView = (ImageView) trackDialog.findViewById(R.id.track_cover);
		titleView = (TextView) trackDialog.findViewById(R.id.track_title2);
		artistView = (TextView) trackDialog.findViewById(R.id.track_artist2);
		albumView = (TextView) trackDialog.findViewById(R.id.track_album2);
		yearView = (TextView) trackDialog.findViewById(R.id.track_year2);
		trackView = (TextView) trackDialog.findViewById(R.id.track_track2);
		genreView = (TextView) trackDialog.findViewById(R.id.track_genre2);
		samplerateView = (TextView) trackDialog
				.findViewById(R.id.track_samplerate2);
		bitrateView = (TextView) trackDialog.findViewById(R.id.track_bitrate2);
		lengthView = (TextView) trackDialog.findViewById(R.id.track_length2);
		commentView = (TextView) trackDialog.findViewById(R.id.track_comment2);

		filterText = (EditText) findViewById(R.string.search_box);

		getListView().setTextFilterEnabled(true);

		error_class_Handler = new Handler();
		viewHandler = new Handler();

		activity = this;

		getListView().setFastScrollEnabled(true);

		// hide keyboard when changing tab
		main.getActivityTabHost().setOnTabChangedListener(
				new OnTabChangeListener() {
					@Override
					public void onTabChanged(String tabId) {
						InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
						imm.hideSoftInputFromWindow(
								RemoteControlPlaylist.filterText
										.getWindowToken(), 0);
					}

				});
	}

	static Runnable update = new Runnable() {
		public void run() {
			if (main.getSocket() == null) {
				Settings.playlist = new PlaylistElement[0];
				viewHandler.post(initialize);
			}

			try { // UPDATE LISTVIEW

				adapter.notifyDataSetChanged();
				activity.getListView().invalidate();

				// SCROLL TO CURRENT PLAYLIST POSITION
				if (RemoteControlPlaylist.touchmode == false) {
					if (Settings.playlistPosition == 0)
						RemoteControlPlaylist.activity.getListView()
								.setSelection(0);
					else
						RemoteControlPlaylist.activity.getListView()
								.setSelection(Settings.playlistPosition - 1);
				}
			} catch (Exception e) {
			}

		}
	};

	static void refreshItems() {
		try {
			viewHandler.post(update);
		} catch (Exception e) {
		}
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		PlaylistElement item = adapter.getItem(position);
		int pos = item.position;

		SendClass.queueOut.add("playlistitem_".concat(String.valueOf(pos)));
		main.getConnectionHandler().post(main.pause_runnable);
	}

	@Override
	public void onUserInteraction() {
		super.onUserInteraction();

		touchmode = true;

		touchModeHandler.removeCallbacks(unsetTouchMode);
		touchModeHandler.postDelayed(unsetTouchMode, 5000);
	}

	final Runnable unsetTouchMode = new Runnable() {
		public void run() {
			touchmode = false;
		}
	};

	@Override
	protected void onDestroy() {
		super.onDestroy();
		filterText.removeTextChangedListener(filterTextWatcher);
	}

	final static Runnable RefreshDialogTextViews = new Runnable() {
		public void run() {

			try {
				titleView.setText(title);
				artistView.setText(artist);
				albumView.setText(artist);
				yearView.setText(year);
				trackView.setText(track);
				genreView.setText(genre);
				samplerateView.setText(samplerate);
				bitrateView.setText(bitrate);
				lengthView.setText(length);
				commentView.setText(comment);
			} catch (Exception e) {
			}

		}
	};

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

	// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// CONTEXT MENU
	// ////////////////////////////////////////////////

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {

		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) menuInfo;

		menu.setHeaderTitle("Options");

		menu.add(0, 0, 0, "Play");

		PlaylistElement item = adapter.getItem(info.position);
		int pos = item.position;

		if (Settings.Queue.contains(pos))
			menu.add(0, 1, 0, "Remove from queue");
		else
			menu.add(0, 1, 0, "Enqueue");

		menu.add(0, 2, 0, "Track info");

	}

	@Override
	public boolean onContextItemSelected(MenuItem menuItem) {
		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) menuItem
				.getMenuInfo();

		PlaylistElement item = adapter.getItem(info.position);
		int pos = item.position;

		switch (menuItem.getItemId()) {
		case 0:
			SendClass.queueOut.add("playlistitem_".concat(String.valueOf(pos)));
			main.getConnectionHandler().post(main.pause_runnable);
			break;
		case 1:

			if (Settings.Queue.contains(info.position)) {
				SendClass.queueOut.add("remqueue_".concat(String
						.valueOf(Settings.Queue.indexOf(pos))));
				Settings.Queue.remove(Settings.Queue.indexOf(pos));
				refreshItems();
			} else {
				SendClass.queueOut.add("enqueue_".concat(String.valueOf(pos)));
				Settings.Queue.add(pos);
				refreshItems();
			}
			break;

		case 2:
			SendClass.queueOut.add("trackInfo_".concat(String.valueOf(pos)));

			viewHandler.post(SetEmptyCover);

			trackDialog.show();

			break;
		}

		return true;
	}

}