package com.study.remote;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

	private LinearLayout lightViews;
	private TextView viewIpAddress;
	private ListView viewList;
	private SetupDao setupDao;
	private int[] status = new int[] { 0, 0 };
	private List<Button> switchList;
	private List<ImageView> lightList;
	MyListAdapter myAdapter = null;
	int inx = 1;
	
	/*
		//client发送原始数据 
		//接收的原始数据格式	
		msgtype  1 bytes 
		op           1 byte
		op_data   4 bytes
		
		devid       4 bytes
		unit_type 1 byte
		unit_num 1 byte
		total 1+1+4+4+1+1 = 12 bytes
	 */
	//控制灯命令
	private char[] sendCommandLed4 = new char[]{1,1,0,0,0,0,0,0,0,0,1,4};
	private char[] sendCommandLed5 = new char[]{1,1,0,0,0,0,0,0,0,0,1,5};
	private char[] sendCommandLedstatus = new char[]{1,2,0,0,0,0,0,0,0,0,1,0};
	
	final String LOGTAG = "MainActivity";
	Handler mHandler;
	MsgRev m_msgRev;
    private String[] mListStr = { "123", "23", "25", "3333",
    "xuanyusong@gmail.com" };
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		getActionBar().setDisplayHomeAsUpEnabled(true);

		lightViews = (LinearLayout) findViewById(R.id.lightViews);
		viewIpAddress = (TextView) findViewById(R.id.viewIpAddress);
		viewList = (ListView) findViewById(R.id.listView1);
		
		setupDao = SetupDao.getInstance(this);

		switchList = new ArrayList<Button>();
		lightList = new ArrayList<ImageView>();
		
		// 控制小Co的Led1的按钮
		LinearLayout layout0 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_light, null);
		lightList.add((ImageView) layout0.findViewById(R.id.imageView));
		TextView textView0 = (TextView) layout0.findViewById(R.id.textView);
		textView0.setText("Led4");
		switchList.add((Button) layout0.findViewById(R.id.button));
		lightViews.addView(layout0);
		
		// 控制小Co的Led2的按钮
		LinearLayout layout1 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_light, null);
		lightList.add((ImageView) layout1.findViewById(R.id.imageView));
		TextView textView1 = (TextView) layout1.findViewById(R.id.textView);
		textView1.setText("Led5");
		switchList.add((Button) layout1.findViewById(R.id.button));
		lightViews.addView(layout1);
		
		//list view
		myAdapter = new MyListAdapter(this,R.layout.arraylist);
		viewList.setAdapter(myAdapter);

/*		
		// 控制小En的Led1的按钮
		LinearLayout layout2 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_light, null);
		lightList.add((ImageView) layout2.findViewById(R.id.imageView));
		TextView textView2 = (TextView) layout2.findViewById(R.id.textView);
		textView2.setText("小En Led1");
		switchList.add((Button) layout2.findViewById(R.id.button));
		lightViews.addView(layout2);
		
		// 控制小En的Led2的按钮
		LinearLayout layout3 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_light, null);
		lightList.add((ImageView) layout3.findViewById(R.id.imageView));
		TextView textView3 = (TextView) layout3.findViewById(R.id.textView);
		textView3.setText("小En Led2");
		switchList.add((Button) layout3.findViewById(R.id.button));
		lightViews.addView(layout3);
		
		// 控制小Co的继电器的按钮
		LinearLayout layout4 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_relay, null);
		lightList.add((ImageView) layout4.findViewById(R.id.relayView));
		TextView textView4 = (TextView) layout4.findViewById(R.id.textView);
		textView4.setText("继电器");
		switchList.add((Button) layout4.findViewById(R.id.button));
		lightViews.addView(layout4);
		
		// 控制小Co的蜂鸣器的按钮
		LinearLayout layout5 = (LinearLayout) getLayoutInflater().inflate(
				R.layout.view_beep, null);
		lightList.add((ImageView) layout5.findViewById(R.id.beepView));
		TextView textView5 = (TextView) layout5.findViewById(R.id.textView);
		textView5.setText("蜂鸣器");
		switchList.add((Button) layout5.findViewById(R.id.button));
		lightViews.addView(layout5);
*/		
		viewIpAddress.setText("Server Ip:" + setupDao.getString("IpAddress"));	
		mHandler = new Handler()  
		{  
		        public void handleMessage(Message msg)  
		        {  
		        	Log.d(LOGTAG, "handleMessage......");  
		        	super.handleMessage(msg);
		            //更新UI按钮
		        	/*
		            switch(msg.what)  
		            {  
		            case 4:  
		                break; 
		            case 5:
		                break; 
		            default:  
		                break;		                		               		
		            }  		
		            */
		        	 String msgtype = msg.getData().getString("msgtype");
		        	 String op  = msg.getData().getString("op");
		        	 String devid = msg.getData().getString("devid");
		        	 String unit_type  = msg.getData().getString("unit_type");		            
		        	 String unit_num = msg.getData().getString("unit_num");
		        	 String result  = msg.getData().getString("result");
					 Log.v(LOGTAG,"unit_type="+unit_type
								+" unit_num="+unit_num +" result="+result);		        	 
		        	 if(Integer.parseInt(unit_type) == 0) //按键
		        	 {
		        		 String unit_name="";
		        		 switch(Integer.parseInt(unit_type))
		        		 {
		        		 	case 0:
		        		 		unit_name="按键";
		        		 		break;
		        		 	case 1:
		        		 		unit_name="灯";
		        		 	default:
		        		 		break;
		        		 }	
						SimpleDateFormat   sDateFormat   =   new   SimpleDateFormat("yyyy-MM-dd hh:mm:ss");   
						String   date   =   sDateFormat.format(new   java.util.Date());

						myAdapter.addItem(date,
								Integer.toString(inx ++)+ " 设备:"+devid+" 类型:"+unit_name + " 编号:"+ unit_num +" 按下 ");
		        			
		        	 }
		        	 else if(Integer.parseInt(unit_type) == 1) //led
		        	 {

		        		 if(Integer.parseInt(unit_num) == 4)
		        		 {
		        			 if(Integer.parseInt(result) == 0)
		        			 {
		     					switchList.get(0).setBackgroundResource(R.drawable.switch_off);
								lightList.get(0).setBackgroundResource(R.drawable.light_off);			        			 
		        			 }
		        			 else
		        			 {
		     					switchList.get(0).setBackgroundResource(R.drawable.switch_on);
								lightList.get(0).setBackgroundResource(R.drawable.light_on);			   
		        			 }
		        		 }
		        		 else if(Integer.parseInt(unit_num) == 5)
		        		 {

		        			 if(Integer.parseInt(result) == 0)
		        			 {
			     				switchList.get(1).setBackgroundResource(R.drawable.switch_off);
								lightList.get(1).setBackgroundResource(R.drawable.light_off);		        			 
		        			 }
		        			 else
		        			 {
			     				switchList.get(1).setBackgroundResource(R.drawable.switch_on);
								lightList.get(1).setBackgroundResource(R.drawable.light_on);		   
		        			 }
		        		 }
		        	 
		        	 }
		        }  
		   };  
		   m_msgRev = new MsgRev(this,mHandler);
		   m_msgRev.start();
		   
		   SimpleDateFormat   sDateFormat   =   new   SimpleDateFormat("yyyy-MM-dd hh:mm:ss");   
		   String   date   =   sDateFormat.format(new   java.util.Date());		   
		   myAdapter.addItem(date,"按键测试");
	}

	@Override
	protected void onResume() {
		if (TextUtils.isEmpty(setupDao.getString("IpAddress"))) {
			inputIpAddress();
		}else{
			new StatusTask(MainActivity.this,"Query Status").execute("123");
		}
		super.onResume();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			finish();
			return true;
		case R.id.setting:
			inputIpAddress();
			return true;
		case R.id.refresh:
			new StatusTask(MainActivity.this,"Query Status").execute("123");
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
     /*ListAdapter */
	 public class MyListAdapter extends ArrayAdapter<Object> {
			int mTextViewResourceID = 0;

		
			private Context mContext;
			private ArrayList<String> item_text;
			private ArrayList<String> item_title;
					
			public MyListAdapter(Context context, int textViewResourceId) {
			    super(context, textViewResourceId);
			    mTextViewResourceID = textViewResourceId;
			    mContext = context;
			    item_text = new ArrayList<String>();
			    item_title = new ArrayList<String>();
			}

			private int[] colors = new int[] { 0xff626569, 0xff4f5257 };
			
			@Override
			public int getCount() {
			    return item_text.size();
			    //return mListStr.length;
			}
			
			@Override
			public boolean areAllItemsEnabled() {
			    return false;
			}
			
			@Override
			public Object getItem(int position) {
			    return item_text.get(position); //position;
			    //return position;
			}
			
			@Override
			public long getItemId(int position) {
			    return position;
			}
				
			@Override
			public View getView(final int position, View convertView, ViewGroup parent) {

			    if (convertView == null) {
				convertView = LayoutInflater.from(mContext).inflate(
					mTextViewResourceID, null);
				}

			    ImageView iamge = (ImageView) convertView.findViewById(R.id.array_image);
			    TextView title = (TextView) convertView.findViewById(R.id.array_title);
			    TextView text = (TextView) convertView.findViewById(R.id.array_text);
			    Button button = (Button)convertView.findViewById(R.id.array_button);
				button.setOnClickListener(new OnClickListener() {				    
				    @Override
				    public void onClick(View arg0) {
					Toast.makeText(MainActivity.this,"您点击的第"+position +"个按钮", Toast.LENGTH_LONG).show();
					
				    }
				});			    			
			    int colorPos = position % colors.length;
			    convertView.setBackgroundColor(colors[colorPos]);		
			    text.setText(item_text.get(position)/*mListStr[position]*/);
			    title.setText(item_title.get(position));
			    //text.setText(mListStr[position]);
			    
			    /*
			    if(colorPos == 0)
				iamge.setImageResource(R.drawable.jay);
			    else
				iamge.setImageResource(R.drawable.image);
				*/
			    return convertView;
			}
			
			/** 
			     * 添加列表项 
			     * @param item 
			*/  
			 public void addItem(String title,String txt) {  
			    	item_text.add(txt);  
			    	item_title.add(title);
			    	notifyDataSetChanged(); 
			    }  	

		 }
	 
     /*ListAdapter */ 
	 
	 
	
	private void inputIpAddress() {
		final EditText editIpAddress = new EditText(this);
		editIpAddress.setText(setupDao.getString("IpAddress"));
		new AlertDialog.Builder(this).setTitle("请输入IP地址")
				.setIcon(android.R.drawable.ic_dialog_info)
				.setView(editIpAddress)
				.setPositiveButton("确定", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface arg0, int arg1) {
						setupDao.putString("IpAddress", editIpAddress.getText()
								.toString());
						viewIpAddress.setText("Server Ip:"
								+ setupDao.getString("IpAddress"));
						new StatusTask(MainActivity.this,"Query Status").execute("123");
					}
				}).setNegativeButton("取消", null).show();
	}

	public void switch_click(View v) {
		if (TextUtils.isEmpty(setupDao.getString("IpAddress"))) {
			Toast.makeText(this, "Please setting ip address!",
					Toast.LENGTH_LONG).show();
			return;
		}
		int index = switchList.indexOf(v);
		if (status[index] == 0) {
			new SendTask(this, "Sending", index).execute(1 + "");
		} else {
			new SendTask(this, "Sending", index).execute(0 + "");
		}
	}

	
	
	//task
	private class StatusTask extends BaseTask {

		public StatusTask(Context context, String message) {
			super(context, message);
		}

		@Override
		protected String doInBackground(String... params) {
			Socket socket = null;
			Scanner scanner = null;
			PrintWriter out = null;
			try {
				socket = new Socket();
				SocketAddress socAddress = new InetSocketAddress(setupDao.getString("IpAddress"),6666); 
			    socket.connect(socAddress, 5000);
				Log.v(LOGTAG,"StatusTask:Client is created! site:"+setupDao.getString("IpAddress")+" port:"+"6666");
				//scanner = new Scanner(socket.getInputStream());
				out = new PrintWriter(socket.getOutputStream());
				out.print(sendCommandLedstatus);
				out.flush();
				
				/*
				String line = scanner.nextLine();
				if (!TextUtils.isEmpty(line)) {
					String[] tokens = line.split(",");
					if (tokens.length == 2) {
						for (int i = 0; i < tokens.length; i++) {
							status[i] = Integer.parseInt(tokens[i]);
						}
					}
				}
				*/

			}
			 catch (UnknownHostException e){
				e.printStackTrace();
             }	catch (IOException e) {
				e.printStackTrace();
				return e.getMessage();
			} finally {
				if (out != null)
					out.close();
				if (scanner != null)
					scanner.close();
				if (socket != null)
					try {
						socket.close();
					} catch (IOException e) {
						return e.getMessage();
					}
			}
			return "ok";
		}

		@Override
		protected void onPostExecute(String result) {
			super.onPostExecute(result);
			if(result.equals("ok")){
				for(int i=0;i < status.length;i++){
					if(status[i]==0){
						switchList.get(i).setBackgroundResource(R.drawable.switch_off);
						if(i < 2)
						{
							lightList.get(i).setBackgroundResource(R.drawable.light_off);	
						}
					}else{
						switchList.get(i).setBackgroundResource(R.drawable.switch_on);
						if(i < 2)
						{
							lightList.get(i).setBackgroundResource(R.drawable.light_on);	
						}						
					}
				}
			}
			else{

				Toast.makeText(MainActivity.this, result, Toast.LENGTH_SHORT)
						.show();
		
			}
		}
	}

	private class SendTask extends BaseTask {

		private int index;

		public SendTask(Context context, String message, int index) {
			super(context, message);
			this.index = index;
		}

		@Override
		protected String doInBackground(String... params) {
			Socket socket = null;
			Scanner scanner = null;
			PrintWriter out = null;
			try {
				socket = new Socket();
				SocketAddress socAddress = new InetSocketAddress(setupDao.getString("IpAddress"),6666); 
			    socket.connect(socAddress, 5000);
				Log.v(LOGTAG,"SendTask:Client is created! site:"+setupDao.getString("IpAddress")+" port:"+"6666");
				//scanner = new Scanner(socket.getInputStream());
				out = new PrintWriter(socket.getOutputStream());
				//for(int i=0;i<sendCommandLed4.length;i++)
				{
					if (index == 0)
					{
						out.print(sendCommandLed4);	
						out.flush();
					}
					else if(index == 1)
					{
						out.print(sendCommandLed5);	
						out.flush();						
					}
				}
				//out.print("set:"+index+"="+params[0]);
				//out.flush();
				
				//读取数据
				//String line = scanner.nextLine();	             
				//BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));     
				//String msg = br.readLine();        	             
                //发送数据    
				//PrintWriter out = new PrintWriter( new BufferedWriter( new OutputStreamWriter(socket.getOutputStream())),true);          
				//out.println(message); 

			} 
				catch (UnknownHostException e){
					e.printStackTrace();
	             }catch (IOException e) {
				e.printStackTrace();
				return e.getMessage();
			} finally {
				if (scanner != null)
					scanner.close();
				if (out != null)
					out.close();
				
				if (socket != null)
					try {
						socket.close();
					} catch (IOException e) {
						return e.getMessage();
					}
				
			}
			return "ok";
		}

		@Override
		protected void onPostExecute(String result) {
			super.onPostExecute(result);
			if (!result.equalsIgnoreCase("ok")) {
				Toast.makeText(MainActivity.this, result, Toast.LENGTH_SHORT)
						.show();
			} else {
				//new StatusTask(MainActivity.this,"Query Status").execute("123");
			}
		}
	}
	
	/*
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {  
		// TODO Auto-generated method stub  
		if(keyCode==KeyEvent.KEYCODE_BACK&&event.getRepeatCount()==0){  
		    //需要处理  
			Log.v(LOGTAG,"onKeyDown:back_key");
			//finish();
			//return true;
		}  
		    return false;  
		}
	*/
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		Log.v(LOGTAG,"onDestroy");
		m_msgRev.interrupt();
		finish();
		System.exit(0);
	}  
}
