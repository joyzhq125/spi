package com.study.remote;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.Scanner;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

/*
//
typedef enum tagE_MsgType
{

    E_MSG_PROACT = 0, //
    
    E_MSG_REQ,             //
    E_MSG_REQ_NO_RSP,  //
    
    E_MSG_REQ_LOCAL,   // 
    E_MSG_REQ_LOCAL_NO_RSP, //
    
    E_MSG_RSP_LOCAL,  //
    E_MSG_MAX

}E_MsgType;



typedef enum tagE_UnitType
{
    //
    E_UNIT_KEY = 0,
    E_UNIT_LAMP,
    E_UNIT_IR ,      
    
    E_UNIT_TEMP, 
    E_UNIT_HUM,   
    E_UNIT_MAX
}E_UnitType;

typedef enum tagE_OP
{
    E_OP_READ = 0,
    E_OP_WRITE,
    E_OP_GETSTATUS,
    E_OP_REBOOT,
    E_OP_REBOOT_DAEMON,
    E_OP_READ_STATUS,
    E_OP_MAX
    
}E_OP;


*/

public class MsgRev extends Thread {
	
	final String LOGTAG = "MsgRev";
	final int    RECVLEN = 12;
	
	char m_msgtype;
	char m_op;
	int  m_devid;
	char m_unit_type;
	char m_unit_num;
	int  m_result;

	Socket socket = null;
	BufferedReader br = null;
	private SetupDao setupDao;
	Handler m_handler;
	Context m_context;
	public MsgRev()
	{
		super(); 
	}

    public MsgRev(Context context,Handler handler) { 
        super();
        m_context = context;
        m_handler = handler;  

    }  
	int decode4bits(char c1,char c2,char c3,char c4)
	{
		int ret = 0;
		ret = c1<<24|c2<<16|c3<<8|c4;
		return ret;
	}
	
	//读取消息，通知主线程更新
	public void run()
	{
		
		socket = new Socket();
		setupDao = SetupDao.getInstance(m_context);
		SocketAddress socAddress = new InetSocketAddress(setupDao.getString("IpAddress"),6666); 
	    try {
			socket.connect(socAddress, 5000);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Log.v(LOGTAG,"read Client is created! site:"+socket.getInetAddress());
		try {
			int idx = 0;

			BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			char []read_buf = new char[RECVLEN]; 
			while(!this.isInterrupted())
			{
		    	int len = 0;
				int readnum = 0;
		    	for(idx = 0;idx < RECVLEN;idx ++)
		    	{
		    		read_buf[idx] = 0;
		    	}
		    	while(len < RECVLEN){
		    		if ((readnum = br.read(read_buf,len,RECVLEN - len)) > 0)
		    		{
		    			len += readnum; // read msg.
		    		}
		    		else
		    		{
		    			break;
		    		}
		    	}
		    	if(len != RECVLEN)
		    	{
		    		Log.v(LOGTAG,"msg read length="+String.valueOf(len)+"not "+String.valueOf(RECVLEN));
		    		continue;
		    	}
		    	
				Log.v(LOGTAG,"msg read length="+String.valueOf(len));
				for(int i = 0;i < len; i++)
				{
					Log.v(LOGTAG,"msg["+String.valueOf(i)+"]="+Integer.toString(read_buf[i]));
				}
	
				/*
				msgtype  1 bytes 
				op           1 byte

				devid       4 bytes
				unit_type 1 byte
				unit_num 1 byte	 
				result      4 bytes
				total 1+1+4+1+1+4 = 12 bytes
				*/
				/*
				0 0 0 0 0 0 0 0 0 0 0 49  按键1
				0 0 0 0 0 0 0 0 0 0 0 50  按键2
				0 0 0 0 0 0 0 0 0 0 0 51  按键3
				 * 
				 * 
				 */
				//解析
				m_msgtype = read_buf[0];
				m_op =  read_buf[1];
				m_devid = decode4bits(read_buf[2],read_buf[3],read_buf[4],read_buf[5]);
				m_unit_type = read_buf[6];
				m_unit_num = read_buf[7];
				m_result = decode4bits(read_buf[8],read_buf[9],read_buf[10],read_buf[11]);
				
				Log.v(LOGTAG,"msgtype="+Integer.toString(m_msgtype)+" op="+Integer.toString(m_op)
						+" devid="+Integer.toString(m_devid)+" unit_type="+Integer.toString(m_unit_type)
						+" unit_num="+Integer.toString(m_unit_num)+" result="+Integer.toString(m_result));
				
				
		        Message msg=new Message();  
		        msg.what=0;  
	            Bundle bundle = new Bundle();    
                bundle.putString("msgtype",Integer.toString(m_msgtype));  
                bundle.putString("op",Integer.toString(m_op)); 
                bundle.putString("devid",Integer.toString(m_devid)); 
                bundle.putString("unit_type",Integer.toString(m_unit_type)); 
                bundle.putString("unit_num",Integer.toString(m_unit_num)); 
                bundle.putString("result",Integer.toString(m_result)); 
                msg.setData(bundle); 
	            m_handler.sendMessage(msg);  
				
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally{
		if (br != null)
			try {
				br.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
	
		if (socket != null)
			try {
				Log.v(LOGTAG,"close socket");
				socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		
		}

	}
	
}
