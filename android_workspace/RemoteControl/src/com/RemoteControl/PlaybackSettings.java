package com.RemoteControl;

public class PlaybackSettings {

	private int isPlaying = 0; // 0: not playing 1: playing 3: paused

	private String title = new String();
	private char repeat_status;
	private char shuffle_status;
	private int length, progress, volume, samplerate;
	private char[] bitrate = new char[4];

	private long startTime;
	private long current_millis;
	private int current_seconds;
	private int current_minutes;

	public synchronized void setCurrent_seconds(int current_seconds) {
		this.current_seconds = current_seconds;
	}

	public synchronized int getCurrent_seconds() {
		return current_seconds;
	}

	public synchronized void setCurrent_millis(long current_millis) {
		this.current_millis = current_millis;
	}

	public synchronized long getCurrent_millis() {
		return current_millis;
	}

	public synchronized void setStartTime(long startTime) {
		this.startTime = startTime;
	}

	public synchronized long getStartTime() {
		return startTime;
	}

	public synchronized void setCurrent_minutes(int current_minutes) {
		this.current_minutes = current_minutes;
	}

	public synchronized int getCurrent_minutes() {
		return current_minutes;
	}

	public synchronized void setRepeat_status(char repeat_status) {
		this.repeat_status = repeat_status;
	}

	public synchronized char getRepeat_status() {
		return repeat_status;
	}

	public synchronized void setShuffle_status(char shuffle_status) {
		this.shuffle_status = shuffle_status;
	}

	public synchronized char getShuffle_status() {
		return shuffle_status;
	}

	public synchronized void setTitle(String title) {
		this.title = title;
	}

	public synchronized String getPlaybackTitle() {
		return title;
	}

	public synchronized void setLength(int length) {
		this.length = length;
	}

	public synchronized int getLength() {
		return length;
	}

	public synchronized void setBitrate(char[] bitrate) {
		this.bitrate = bitrate;
	}

	public synchronized char[] getBitrate() {
		return bitrate;
	}

	public synchronized void setSamplerate(int samplerate) {
		this.samplerate = samplerate;
	}

	public synchronized int getSamplerate() {
		return samplerate;
	}

	public synchronized void setVolume(int volume) {
		this.volume = volume;
	}

	public synchronized int getVolume() {
		return volume;
	}

	public synchronized void setPlaybackProgress(int progress) {
		this.progress = progress;
	}

	public synchronized int getPlaybackProgress() {
		return progress;
	}

	public synchronized void setIsPlaying(int isPlaying) {
		this.isPlaying = isPlaying;
	}

	public int getIsPlaying() {
		return isPlaying;
	}

}
