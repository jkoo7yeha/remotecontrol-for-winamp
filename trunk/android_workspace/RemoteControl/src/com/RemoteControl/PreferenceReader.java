package com.RemoteControl;

import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.telephony.PhoneStateListener;

public class PreferenceReader {

	public void getPrefs() {
		SharedPreferences prefs = PreferenceManager
				.getDefaultSharedPreferences(main.getActivity()
						.getBaseContext());
		SharedPreferences.Editor editor = prefs.edit();

		Settings.liststatus[0] = prefs.getString("0", "192.168.2.50");
		Settings.liststatus[1] = prefs.getString("1", "50000");
		Settings.liststatus[2] = prefs.getString("2", "3000");
		Settings.liststatus[3] = prefs.getBoolean("3", false);
		Settings.liststatus[4] = prefs.getString("4", "Dim");
		Settings.liststatus[5] = prefs.getBoolean("5", true);

		// register IncomingCallListener
		if (Settings.liststatus[5].equals(true))
			main.getTelephonyManager().listen(main.getPhoneListener(),
					PhoneStateListener.LISTEN_CALL_STATE);

		// auto connect
		if (Settings.liststatus[3].equals(true))
			main.getActivity().onClick(main.getButton_connect());

		// first start
		if (prefs.getString("first_start", "activated").equals("activated")) {
			main.showHelp();

			editor.putString("first_start", "deactivated");
			editor.commit();
		}

	}

}
