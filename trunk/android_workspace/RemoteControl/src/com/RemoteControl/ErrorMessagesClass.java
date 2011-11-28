package com.RemoteControl;

public class ErrorMessagesClass {

	final static Runnable file_read_error = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("could not read from settings file!");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable file_write_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("could not write to settings file!");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable file_create_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("could not create settings file!");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable connect_timeout = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("timeout");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable port_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("port must be between 0 and 65535");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable ip_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("could not find IP address");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable channel_error = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("socket has unassociated channel in non-blocking mode");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable endpointer_error = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("endpointer is null or SocketAddress subclass not supported by socket");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable unknown_socket_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("connection refused");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable output_stream_state = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("while creating output stream or socket has invalid state");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable input_stream_state = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("while creating input stream or socket has invalid state");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable reader_error = new Runnable() {
		public void run() {
			ToastClass.toast_error
					.setText("reader is closed or some other I/O error occured");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable disconnected_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("connection has to be established");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable conversion_error = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("conversion failed");
			ToastClass.toast_error.show();
		}
	};

	final static Runnable unsupportedEncodingError = new Runnable() {
		public void run() {
			ToastClass.toast_error.setText("encoding unicode string failed");
			ToastClass.toast_error.show();
		}
	};
}
