package com.RemoteControl;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class UTF8Reader {

	/**
	 * Reads UTF-8 character data; lines are terminated with '\n'.
	 * 
	 * @param in
	 *            InputStream to read from
	 * @return String
	 * @throws IOException
	 *             Error reading from stream
	 */
	public static synchronized String readLine(InputStream in)
			throws IOException {
		ByteArrayOutputStream buffer = new ByteArrayOutputStream();
		while (true) {
			int b = in.read();
			if (b < 0) {
				throw new IOException("Data truncated");
			}
			if (b == 0x0A) {
				break;
			}
			buffer.write(b);
		}
		return new String(buffer.toByteArray(), "UTF-8");
	}

}
