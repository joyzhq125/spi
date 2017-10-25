package com.study.remote;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class SetupDao {
	
	private static SetupDao setupDao=null;
	private SharedPreferences preference;
	
	private SetupDao() {
		
	}
	
	public static SetupDao getInstance(Context context){
		if(setupDao==null){
			setupDao=new SetupDao();
			setupDao.preference=context.getSharedPreferences("setup", Context.MODE_PRIVATE);
		}
		return setupDao;
	}

	public void putString(String name,String value){
		Editor editor=preference.edit();
		editor.putString(name, value);
		editor.commit();
	}
	
	public String getString(String name){
		return preference.getString(name, "");
	}

	public int getInt(String name) {
		return preference.getInt(name, 0);
	}

	public void putInt(String name, int value) {
		Editor editor=preference.edit();
		editor.putInt(name, value);
		editor.commit();
	}
	
}
