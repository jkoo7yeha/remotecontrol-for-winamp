package com.RemoteControl;

import java.io.IOException;
import java.util.LinkedList;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class CoverReader {

	private LinkedList<Bitmap> coverList = new LinkedList<Bitmap>();

	/**
	 * reads a cover file from the input stream. if the hash of a cover is in
	 * coverHashMap the cover is taken from coverHashMap.
	 * 
	 * @param fileSize
	 *            size of cover to read
	 * @param makeSmall
	 *            if true, the file is resized to 50 px width and put into
	 *            coverHashMap
	 * @return Bitmap
	 * @throws IOException
	 */
	Bitmap readCover(int fileSize, boolean makeSmall) throws IOException {

		Bitmap cover = null;

		if (fileSize > 0) {
			try {
				byte[] imageBytes = new byte[fileSize];

				int length, count = 0;

				// read data
				while (count < fileSize) {
					length = main.getInputStream().read(imageBytes, count,
							fileSize - count);
					count += length;
				}

				// chose: playlist cover (fit to 50 px width and put into
				// coverList) OR overview cover (original size, not put into
				// coverList)
				if (makeSmall) {
					// get cover from coverList or create new bitmap and put
					// into coverHashMap

					if (fileSize < 0)
						cover = coverList.get(-fileSize);
					else {
						BitmapFactory.Options bfOptions = new BitmapFactory.Options();

						if (fileSize < 10000)
							bfOptions.inSampleSize = 1;
						else if (fileSize < 20000)
							bfOptions.inSampleSize = 2;
						else if (fileSize < 40000)
							bfOptions.inSampleSize = 4;
						else if (fileSize < 80000)
							bfOptions.inSampleSize = 8;
						else
							bfOptions.inSampleSize = 16;

						cover = BitmapFactory.decodeByteArray(imageBytes, 0,
								imageBytes.length, bfOptions);

						if (cover != null)
							coverList.add(cover);
					}
				} else
					cover = BitmapFactory.decodeByteArray(imageBytes, 0,
							imageBytes.length);

			} catch (IOException e) {
				e.printStackTrace();

				main.getErrorClassHandler().post(
						ErrorMessagesClass.conversion_error);
			}
		}

		return cover;
	}

	/**
	 * resets the coverHashMap
	 */
	void resetCoverList() {
		coverList.clear();
	}
}
