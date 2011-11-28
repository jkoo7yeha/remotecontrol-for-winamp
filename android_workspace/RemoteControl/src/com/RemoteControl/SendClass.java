package com.RemoteControl;

import java.io.DataOutputStream;
import java.io.IOException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class SendClass {

	static String parameter;
	static volatile BlockingQueue<String> queueOut = new LinkedBlockingQueue<String>();

	static DataOutputStream outToServer;

	static Thread t;

	static void start() {

		queueOut.clear();

		t = new Thread() {
			public void run() {

				// send command

				while (outToServer != null) {
					try {
						parameter = queueOut.take();
					} catch (InterruptedException e1) {
						e1.printStackTrace();

						// DISPLAY ERROR
						ToastClass.toast_error
								.setText("ERROR: queue interrupted!");
						ToastClass.toast_error.show();

						break;
					}

					if (parameter.equals("destroy"))
						break;

					try {
						outToServer.writeBytes(parameter);

					} catch (IOException e) {
						e.printStackTrace();

						// DISPLAY ERROR
						ToastClass.toast_error
								.setText("ERROR: error while writing to target stream");
						ToastClass.toast_error.show();
					} catch (Exception e) { // parameter DataOutputStream is
											// null
						e.printStackTrace();

						// DISPLAY ERROR
						ToastClass.toast_error
								.setText("ERROR: establish connection first!");
						ToastClass.toast_error.show();
					}

				}

			}
		};
		t.start();
	}

}
