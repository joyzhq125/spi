package com.study.remote;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;

public abstract class BaseTask extends AsyncTask<String, Integer, String> {

	private ProgressDialog progressDialog = null;

	public BaseTask(Context context, String message) {
		this(context, message, true);
	}

	public BaseTask(Context context, String message, boolean showProcess) {
		if (showProcess) {
			progressDialog = new ProgressDialog(context);
			progressDialog.setMessage(message);
			progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
			progressDialog.setCanceledOnTouchOutside(false);
		}
	}

	@Override
	protected void onPreExecute() {
		super.onPreExecute();
		if (progressDialog != null) {
			progressDialog.show();
		}
	}

	@Override
	protected void onPostExecute(String result) {
		if (progressDialog != null) {
			progressDialog.dismiss();
		}
		super.onPostExecute(result);
	}



}
