package com.RemoteControl;

import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

public class IncomingCallListener extends PhoneStateListener {

	@Override
	public void onCallStateChanged(int state, String incomingNumber) {
		if (state == TelephonyManager.CALL_STATE_RINGING
				&& main.getSocket() != null
				&& main.getPlaybackSettings().getIsPlaying() == 1) {
			main.getPlaybackSettings().setCurrent_millis(
					System.currentTimeMillis()
							- main.getPlaybackSettings().getStartTime());

			try {
				RemoteControlOverview.timertask.cancel();
				RemoteControlOverview.timertask.cancel = true;
				RemoteControlOverview.timer.cancel();
			} catch (Exception e) {
			}

			main.getPlaybackSettings().setIsPlaying(3);

			main.getConnectionHandler().post(main.play_runnable);

			SendClass.queueOut.add("pause");
		}
	}
}
