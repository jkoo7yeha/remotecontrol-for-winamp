package com.RemoteControl;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.LinkedList;
import java.util.Timer;

import android.graphics.Bitmap;
import com.RemoteControl.RemoteControlOverview.UpdateTimeTask;

public class ReceiveClass implements Runnable {

	@Override
	public void run() {
		String message = "";
		CharSequence tmp;

		while (message != null) {

			try {
				message = UTF8Reader.readLine(main.getInputStream());
			} catch (IOException e2) {
				// connection closed
				break;
			}

			if (message != null) {
				if (message.startsWith("alive") == true) {
					// send keep alive packages back
					SendClass.queueOut.add("alive");
				} else if (message.startsWith("isplaying_") == true) {
					try {
						message = message.substring(10, message.length());

						if (!message.equals("3"))
							main.getPlaybackSettings().setIsPlaying(
									Integer.parseInt(message));

					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}

					if (main.getPlaybackSettings().getIsPlaying() != 0)
						main.getConnectionHandler().post(main.pause_runnable);
				} else if (message.startsWith("playlistPosition_") == true) {
					try {
						message = message.substring(17, message.length());

						Settings.playlistPosition = Integer.parseInt(message);
					} catch (Exception e2) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}

					try {
						RemoteControlPlaylist.refreshItems();
					} catch (NullPointerException e1) {
					} // playlist not yet loaded

				} else if (message.startsWith("samplerate_") == true) {
					try {
						message = message.substring(11, message.length());

						main.getPlaybackSettings().setSamplerate(
								Integer.parseInt(message));
					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}
				} else if (message.startsWith("bitrate_") == true) {
					message = message.substring(8, message.length());

					main.getPlaybackSettings()
							.setBitrate(message.toCharArray());
				} else if (message.startsWith("length_") == true) {
					try {
						message = message.substring(7, message.length());

						if (main.getPlaybackSettings().getLength() != Integer
								.parseInt(message) && !message.equals("-1"))
							main.getPlaybackSettings().setLength(
									Integer.parseInt(message));

					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}
				}

				else if (message.startsWith("title_") == true) {
					String title_temp = new String();
					title_temp = message.substring(6, message.length());

					main.getPlaybackSettings().setTitle(title_temp);

					main.getPlaybackSettings().setStartTime(
							System.currentTimeMillis());

					RemoteControlOverview.WinampSettingsHandler
							.post(RemoteControlOverview.UpdateDisplays);

					if (main.getPlaybackSettings().getIsPlaying() == 1) {
						main.getPlaybackSettings().setCurrent_millis(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getStartTime());

						try {
							RemoteControlOverview.timertask.cancel();
							RemoteControlOverview.timertask.cancel = true;
							RemoteControlOverview.timer.cancel();
						} catch (Exception e) {
						}

						main.getPlaybackSettings().setStartTime(
								System.currentTimeMillis());

						RemoteControlOverview.timer = new Timer();
						RemoteControlOverview.timertask = new UpdateTimeTask();
						RemoteControlOverview.timer.schedule(
								RemoteControlOverview.timertask, 0, 1000);
					} else if (main.getPlaybackSettings().getIsPlaying() == 3) {
						main.getPlaybackSettings().setStartTime(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getCurrent_millis());

						RemoteControlOverview.timer = new Timer();
						RemoteControlOverview.timertask = new UpdateTimeTask();
						RemoteControlOverview.timer.schedule(
								RemoteControlOverview.timertask, 0, 1000);

						main.getPlaybackSettings().setIsPlaying(1);
					}

				} else if (message.startsWith("stop") == true) {
					try {
						RemoteControlOverview.timertask.cancel();
						RemoteControlOverview.timertask.cancel = true;
						RemoteControlOverview.timer.cancel();
					} catch (Exception e) {
					}

					RemoteControlOverview.WinampSettingsHandler
							.post(main.stop_runnable);
					RemoteControlOverview.WinampSettingsHandler
							.post(main.play_runnable);

					main.getPlaybackSettings().setIsPlaying(0);
				} else if (message.startsWith("coverLength_") == true) {
					// ///////////////////////////////// COVER
					// ///////////////////////////////////////////////////

					int coverLength = 0;
					CharSequence tmp2;
					// LENGTH
					try {
						tmp2 = message.subSequence(12, message.length());
						coverLength = Integer.parseInt(tmp2.toString());
					} catch (Exception e) {
						e.printStackTrace();

						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);

						break;
					}

					// COVER

					Bitmap cover = null;

					try {
						cover = main.getCoverReader().readCover(coverLength,
								false);
					} catch (IOException e) {
						e.printStackTrace();

						main.getConnect_dialog().dismiss();
						main.getErrorClassHandler().post(
								ErrorMessagesClass.reader_error);
					}

					if (cover != null) {
						RemoteControlOverview.cover = cover;

						RemoteControlOverview.WinampSettingsHandler
								.post(RemoteControlOverview.SetCover);

					} else
						RemoteControlOverview.WinampSettingsHandler
								.post(RemoteControlOverview.SetEmptyCover);

				} else if (message.startsWith("pause") == true) {
					if (main.getPlaybackSettings().getIsPlaying() == 1) {
						main.getPlaybackSettings().setCurrent_millis(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getStartTime());

						try {
							RemoteControlOverview.timertask.cancel();
							RemoteControlOverview.timertask.cancel = true;
							RemoteControlOverview.timer.cancel();
						} catch (Exception e) {
						}

						main.getPlaybackSettings().setIsPlaying(3);

						main.getConnectionHandler().post(main.play_runnable);
					} else if (main.getPlaybackSettings().getIsPlaying() == 3) {
						main.getPlaybackSettings().setStartTime(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getCurrent_millis());

						RemoteControlOverview.timer = new Timer();
						RemoteControlOverview.timertask = new UpdateTimeTask();
						RemoteControlOverview.timer.schedule(
								RemoteControlOverview.timertask, 0, 1000);

						main.getPlaybackSettings().setIsPlaying(1);

						main.getConnectionHandler().post(main.pause_runnable);
					}

				} else if (message.startsWith("shuffle_") == true) {
					tmp = message.subSequence(8, 9);
					main.getPlaybackSettings().setShuffle_status(tmp.charAt(0));

					RemoteControlOverview.WinampSettingsHandler
							.post(RemoteControlOverview.UpdateShuffle);
				} else if (message.startsWith("repeat_") == true) {
					tmp = message.subSequence(7, 8);
					main.getPlaybackSettings().setRepeat_status(tmp.charAt(0));

					RemoteControlOverview.WinampSettingsHandler
							.post(RemoteControlOverview.UpdateRepeat);
				} else if (message.startsWith("volume_") == true) {
					try {
						tmp = message.subSequence(7, message.length());
						main.getPlaybackSettings().setVolume(
								Integer.parseInt(tmp.toString()));
					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}

					RemoteControlOverview.WinampSettingsHandler
							.post(RemoteControlOverview.UpdateVolume);
				} else if (message.startsWith("progress_") == true) {
					try {
						tmp = message.subSequence(9, message.length());
						main.getPlaybackSettings().setPlaybackProgress(
								Integer.parseInt(tmp.toString()));
					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}

					if (main.getPlaybackSettings().getIsPlaying() == 1) {
						try {
							RemoteControlOverview.timertask.cancel();
							RemoteControlOverview.timertask.cancel = true;
							RemoteControlOverview.timer.cancel();
						} catch (Exception e) {
						}

						main.getPlaybackSettings().setStartTime(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getPlaybackProgress());

						RemoteControlOverview.timer = new Timer();
						RemoteControlOverview.timertask = new UpdateTimeTask();
						RemoteControlOverview.timer.schedule(
								RemoteControlOverview.timertask, 0, 1000);
					} else if (main.getPlaybackSettings().getIsPlaying() == 3) {
						try {
							RemoteControlOverview.timertask.cancel();
							RemoteControlOverview.timertask.cancel = true;
							RemoteControlOverview.timer.cancel();
						} catch (Exception e) {
						}

						main.getPlaybackSettings().setIsPlaying(3);

						main.getPlaybackSettings().setStartTime(
								System.currentTimeMillis()
										- main.getPlaybackSettings()
												.getPlaybackProgress());

					}

					RemoteControlOverview.WinampSettingsHandler
							.post(RemoteControlOverview.UpdateProgress);
				} else if (message.startsWith("queue_next") == true) {
					try {
						Settings.Queue.remove(0);
					} catch (Exception e) {
					}

					try {
						RemoteControlPlaylist.refreshItems();
					} catch (Exception e) {
					}
				} else if (message.startsWith("queueRefresh_") == true) // TODO
				{
					try {
						tmp = message.subSequence(13, message.length());
						int numberOfElements = Integer.parseInt((String) tmp);
						int i = 0, itemIndex;

						Settings.Queue = new LinkedList<Object>();

						while (message != null & i < numberOfElements) {

							try {
								message = UTF8Reader.readLine(main
										.getInputStream());
							} catch (IOException e2) {
								// connection closed
								break;
							}

							if (message != null) {
								itemIndex = Integer.parseInt(message) - 1;
								Settings.Queue.add(itemIndex + 1);
							} else {
								main.getErrorClassHandler().post(
										ErrorMessagesClass.conversion_error);
								break;
							}

							i++;
						}

						try {
							RemoteControlPlaylist.refreshItems();
						} catch (Exception e) {
						}
					} catch (Exception e) {
						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);
					}
				} else if (message.startsWith("track_title_") == true) {
					try {
						tmp = message.subSequence(12, message.length());
						RemoteControlPlaylist.title = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_artist_") == true) {
					try {
						tmp = message.subSequence(13, message.length());
						RemoteControlPlaylist.artist = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_album_") == true) {
					try {
						tmp = message.subSequence(12, message.length());
						RemoteControlPlaylist.album = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_year_") == true) {
					try {
						tmp = message.subSequence(11, message.length());
						RemoteControlPlaylist.year = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_track_") == true) {
					try {
						tmp = message.subSequence(12, message.length());
						RemoteControlPlaylist.track = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_genre_") == true) {
					try {
						tmp = message.subSequence(12, message.length());
						RemoteControlPlaylist.genre = (String) tmp;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_samplerate_") == true) {
					try {
						tmp = message.subSequence(17, message.length());
						RemoteControlPlaylist.samplerate = tmp + " Hz";
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_bitrate_") == true) {
					try {
						tmp = message.subSequence(14, message.length());
						RemoteControlPlaylist.bitrate = tmp + " kbit/s";
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_length_") == true) {
					try {
						tmp = message.subSequence(13, message.length());

						int seconds = Integer.parseInt(tmp.toString());
						int minutes = seconds / 60;
						seconds = seconds % 60;

						DecimalFormat df = new DecimalFormat("00");
						String secondsString = df.format(seconds);
						String minutesString = String.valueOf(minutes);

						RemoteControlPlaylist.length = minutesString + ":"
								+ secondsString;
					} catch (Exception e) {
					}
				} else if (message.startsWith("track_comment_") == true) {
					try {
						tmp = message.subSequence(14, message.length());
						RemoteControlPlaylist.comment = (String) tmp;
					} catch (Exception e) {
					}

					// THIS IS THE LAST TEXT ELEMENT TRANSMITTED SO NOW REFRESH
					// TEXTVIEWS
					RemoteControlPlaylist.viewHandler
							.post(RemoteControlPlaylist.RefreshDialogTextViews);
				} else if (message.startsWith("track_coverLength_") == true) {
					// ///////////////////////////////// COVER
					// ///////////////////////////////////////////////////

					int coverLength = 0;
					CharSequence tmp2;
					// LENGTH
					try {
						tmp2 = message.subSequence(18, message.length());
						coverLength = Integer.parseInt(tmp2.toString());
					} catch (Exception e) {
						e.printStackTrace();

						main.getErrorClassHandler().post(
								ErrorMessagesClass.conversion_error);

						break;
					}

					// COVER

					if (coverLength > 0) {

						// cover not yet in coverMap

						try {
							Bitmap bmp = main.getCoverReader().readCover(
									coverLength, false);

							if (bmp != null) {

								RemoteControlPlaylist.cover = bmp;

								RemoteControlPlaylist.viewHandler
										.post(RemoteControlPlaylist.SetCover);
							} else
								RemoteControlPlaylist.viewHandler
										.post(RemoteControlPlaylist.SetEmptyCover);

						} catch (IOException e) {
							e.printStackTrace();

							main.getErrorClassHandler().post(
									ErrorMessagesClass.conversion_error);

							break;
						}

					} else {
						RemoteControlPlaylist.cover = null;
					}
				}

			}
		}

		try {

			// if stream closed
			if (main.getSocket() != null)
				main.getActivity().disconnect();

		} catch (Exception e) {
		}
	}
}
