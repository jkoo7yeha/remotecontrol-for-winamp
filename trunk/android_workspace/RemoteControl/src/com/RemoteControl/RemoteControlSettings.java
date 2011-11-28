package com.RemoteControl;

import com.RemoteControl.R;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.PowerManager;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.telephony.PhoneStateListener;

public class RemoteControlSettings extends PreferenceActivity {

	static RemoteControlSettings activity = null;


	static EditTextPreference pref0, pref1, pref2;
	static CheckBoxPreference pref3, pref5;
	static ListPreference pref4;
	
	String editTextPreference;
	
	SharedPreferences prefs;
	SharedPreferences.Editor editor;
	
	OnPreferenceChangeListener onPreferenceChangeListener = new OnPreferenceChangeListener()
	{
		@Override
		public boolean onPreferenceChange(Preference preference, Object newValue) {

			switch (Integer.parseInt(preference.getKey()))
			{
				case 0:
					Settings.liststatus[0] = (String) newValue;

					// SAVE
			        editor.putString("0", (String) Settings.liststatus[0]);
		            editor.commit();
		            
		            // REFRESH UI
					pref0.setSummary((String) Settings.liststatus[0]);
					pref0.setText((String) Settings.liststatus[0]);
					break;
					
				case 1:
					Settings.liststatus[1] = (String) newValue;

					// SAVE
			        editor.putString("1", (String) Settings.liststatus[1]);
		            editor.commit();
		            
		            // REFRESH UI
					pref1.setSummary((String) Settings.liststatus[1]);
					pref1.setText((String) Settings.liststatus[1]);
					break;
					
				case 2:
					Settings.liststatus[2] = (String) newValue;

					// SAVE
			        editor.putString("2", (String) Settings.liststatus[2]);
		            editor.commit();
		            
		            // REFRESH UI
					pref2.setSummary((String) Settings.liststatus[2]);
					pref2.setText((String) Settings.liststatus[2]);
					break;
					
				case 3:
					Settings.liststatus[3] = newValue;
					
					// SAVE
			        editor.putBoolean("3", (Boolean) Settings.liststatus[3]);
		            editor.commit();
		            
		            // REFRESH UI
					if ((Boolean)Settings.liststatus[3] == true)
						pref3.setChecked(true);
					else
						pref3.setChecked(false);
					break;
					
				case 4:
					Settings.liststatus[4] = newValue;
					
					// SAVE
			        editor.putString("4", (String) Settings.liststatus[4]);
		            editor.commit();
		            
		            // REFRESH UI
		            pref4.setSummary((String) Settings.liststatus[4]);
		            pref4.setValue((String) Settings.liststatus[4]);
					
		            if (Settings.liststatus[4].equals("Normal"))
		            {
		            	try { Settings.wl.release(); } catch (Exception e) {}
		            
		            	Settings.wl = null;
		            }
		            else if (Settings.liststatus[4].equals("Dim"))
		            {
		            	if(main.getSocket() != null)
		            	{
		            		try { Settings.wl.release(); } catch (Exception e) {}
		            		
		            		Settings.wl = Settings.pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "Dim");
		            		Settings.wl.acquire();
		            	}
		            }
		            else if (Settings.liststatus[4].equals("Always on"))
		            {
		            	if(main.getSocket() != null)
		            	{
		            		try { Settings.wl.release(); } catch (Exception e) {}
		            		
		            		Settings.wl = Settings.pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "Always on");
		            		Settings.wl.acquire();
		            	}
		            }
		            
					break;
					
				case 5:
					Settings.liststatus[5] = newValue;
					
					// SAVE
				editor.putBoolean("5", (Boolean) Settings.liststatus[5]);
				editor.commit();
		            
		            // REFRESH UI
					if ((Boolean)Settings.liststatus[5] == true) {	// register phoneListener
						pref5.setChecked(true);
						main.getTelephonyManager().listen(main.getPhoneListener(), PhoneStateListener.LISTEN_CALL_STATE);
					} else {	// unregister phoneListener
						pref5.setChecked(false);
						main.getTelephonyManager().listen(main.getPhoneListener(), PhoneStateListener.LISTEN_NONE);
					}
					break;
			}
			
			return false;
		}
	};
	
	@Override
	public void onStart()
	{
		super.onStart();
		
		pref0.setSummary((String) Settings.liststatus[0]); 
        pref0.setText((String) Settings.liststatus[0]);
        
        pref1.setSummary((String) Settings.liststatus[1]); 
        pref1.setText((String) Settings.liststatus[1]);
        
        pref2.setSummary((String) Settings.liststatus[2]); 
        pref2.setText((String) Settings.liststatus[2]);
        
        if(Settings.liststatus[3].equals(true))
        	pref3.setChecked(true);
        
        pref4.setSummary((String) Settings.liststatus[4]); 
        pref4.setValue((String) Settings.liststatus[4]);
        
        if(Settings.liststatus[5].equals(true))
        	pref5.setChecked(true);
	}
	
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		// initialize editor
		prefs = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
		editor = prefs.edit();
		
		// add preferences
		addPreferencesFromResource(R.layout.settings);
		
		activity = this;
		
		pref0 = (EditTextPreference) findPreference("0");
		pref1 = (EditTextPreference) findPreference("1");
		pref2 = (EditTextPreference) findPreference("2");
		pref3 = (CheckBoxPreference) findPreference("3");
		pref4 = (ListPreference) findPreference("4");
		pref5 = (CheckBoxPreference) findPreference("5");
		
		// check if prefs exist
		if (pref1 == null)
			editor.putString("1", (String) Settings.liststatus[1]);
		if (pref2 == null)
			editor.putString("2", (String) Settings.liststatus[2]);
		if (pref3 == null)
			editor.putBoolean("3", (Boolean) Settings.liststatus[3]);
		if (pref4 == null)
			editor.putString("4", (String) Settings.liststatus[4]);
		if (pref5 == null)
			editor.putBoolean("5", (Boolean) Settings.liststatus[5]);
		
		editor.commit();
		
		
		
		pref0.setNegativeButtonText("Cancel");
		pref0.setPositiveButtonText("OK");
		
		pref1.setNegativeButtonText("Cancel");
		pref1.setPositiveButtonText("OK");
		
		pref2.setNegativeButtonText("Cancel");
		pref2.setPositiveButtonText("OK");
		
		
		pref0.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
		pref1.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
		pref2.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
		pref3.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
		pref4.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
		pref5.setOnPreferenceChangeListener(onPreferenceChangeListener);
		
	}

	
}