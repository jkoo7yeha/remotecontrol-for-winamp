package com.RemoteControl;

import android.graphics.Bitmap;

public class PlaylistElement {
	public PlaylistElement(String title, int pos, Bitmap cover) {
		this.title = title;
		this.position = pos;
		this.cover = cover;
	}

	String title;
	int position;
	Bitmap cover;

	public String toString() {
		return this.title;
	}
}