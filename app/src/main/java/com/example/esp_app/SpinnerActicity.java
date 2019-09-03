package com.example.esp_app;


import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;

public class SpinnerActicity extends AppCompatActivity implements OnItemSelectedListener,View.OnClickListener {
    private Spinner spi = null;//下拉菜单
    private int lastPosition=0;
    private Button ledOn; //开led
    private Button ledOff;  //关led
    private Button connect; //连接按钮
    private EditText Ip;    //ip地址
    private String sip;     //字符串ip
    private EditText port;  //端口号
    private int iport;      //字符端口
    public Socket socket;  //套接字
    private TextView temtxt;
    private TextView wattxt;
    private Timer timer;
    private String TemResult="";//接受温度数据数组
    private String WatResult="";//接受湿度数据数组
    private BufferedReader buf;
    private InputStream in;
    private DataInputStream din;
    public ConnectThread mConnectThread; //Tcp连接线程
    SharedPreferences.Editor editor;
    SharedPreferences sharedPreferences;
    public final static String Pre_IP = "PREF_IP_ADDRESS";
    public final static String Pre_PORT = "PREF_PORT_NUMBER";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sharedPreferences = getSharedPreferences("HTTP_HELPER_PREFS", Context.MODE_PRIVATE);
        editor = sharedPreferences.edit();

        spi = (Spinner) findViewById(R.id.spi_list);
        ledOn = (Button) findViewById(R.id.led_open_but);
        ledOff = (Button) findViewById(R.id.led_close_but);
        connect = (Button) findViewById(R.id.IP_connect_but);
        Ip = (EditText) findViewById(R.id.IP_num);
        port = (EditText) findViewById(R.id.port_num);
        temtxt = (TextView) findViewById(R.id.tem_txt);
        wattxt = (TextView) findViewById(R.id.wat_txt);

        String[] arr = {"数据栏▽", "温湿数据", "烟霾数据", "火焰指数"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, arr);


        spi.setAdapter(adapter);
        spi.setOnItemSelectedListener((AdapterView.OnItemSelectedListener) this);
        ledOn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread() {
                    public void run() {
                        try {
                            mConnectThread.OutPut();
                            mConnectThread.out.write(0);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }.start();

            }
        });
        ledOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread() {
                    public void run() {
                        try {
                            mConnectThread.OutPut();
                            mConnectThread.out.write(1);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }.start();

            }
        });
        connect.setOnClickListener((View.OnClickListener) this);

        Ip.setText(sharedPreferences.getString(Pre_IP, ""));
        port.setText(sharedPreferences.getString(Pre_PORT, ""));

        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                new Thread() {
                    public void run() {
                        try {
                            mConnectThread.OutPut();
                            mConnectThread.out.write(50);
                            mConnectThread.out.flush();
                            mConnectThread.InPut();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }.start();
            }
        }, 0, 5000);
    }

    @Override
    public void onClick(View view) {
        //save the used Ip and port
        String IPAddress = Ip.getText().toString().trim();
        String PortNumber = port.getText().toString().trim();
        editor.putString(Pre_IP, IPAddress);
        editor.putString(Pre_PORT, PortNumber);
        editor.commit();


        switch (view.getId()) {
            case R.id.IP_connect_but: {
                //建立连接
                if (Ip.getText().toString().equals("") || port.getText().toString().equals("")) {
                    Toast.makeText(SpinnerActicity.this, "请输入IP地址和端口号", Toast.LENGTH_LONG).show();
                } else {
                    if (socket == null || !socket.isConnected()) {
                        sip = Ip.getText().toString();
                        iport = Integer.valueOf(port.getText().toString());
                        mConnectThread = new ConnectThread(sip, iport);
                        mConnectThread.start();
                    }
                    if (socket != null && socket.isConnected()) {
                        try {
                            socket.close();
                            socket = null;
                            connect.setText("连接");
                            Toast.makeText(SpinnerActicity.this, "连接已关闭", Toast.LENGTH_LONG).show();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
                break;
            }

        }

    }


    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        String content = spi.getSelectedItem().toString();
        switch (content) {
            case "温湿数据": {
                Intent intent = new Intent(SpinnerActicity.this, HumitureActivity.class);
                startActivity(intent);
                break;
            }

            case "烟霾数据": {
                Intent intent = new Intent(SpinnerActicity.this, HazeActiviy.class);
                startActivity(intent);
                break;
            }

            case "火焰指数": {
                Intent intent = new Intent(SpinnerActicity.this, FireActivity.class);
                startActivity(intent);
                break;
            }
        }
        try {
            Class<?> clazz = AdapterView.class;
            Field field = clazz.getDeclaredField("mOldSelectedPosition");
            field.setAccessible(true);
            field.setInt(spi,AdapterView.INVALID_POSITION);
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    public void onNothingSelected(AdapterView<?> parent) {
    }


    public class ConnectThread extends Thread {
        public String ip;
        public int port;
        public OutputStream out;

        public ConnectThread(String ip, int port) {
            this.ip = ip;
            this.port = port;
        }
        public void InPut  () throws IOException {
            System.out.println("/////");
            in=socket.getInputStream();
            buf=new BufferedReader(new InputStreamReader(in));
            StringBuffer out=new StringBuffer();
            byte[] b=new byte[2048];
            int n=in.read(b);
            while(n!='\n'){
                out.append(new String(b,0,n));
                TemResult+=n;
            }
            System.out.println(TemResult);
            System.out.println("///llllllll//");
//                       TemResult=String.valueOf(buf.read());
            temtxt.setText(TemResult);



        }

        public void OutPut() {
            try {
                out = socket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            try {
                socket = new Socket(ip, port);
                out = socket.getOutputStream();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        connect.setText("断开");
                        Toast.makeText(SpinnerActicity.this, "连接成功", Toast.LENGTH_LONG).show();
                    }
                });

            } catch (IOException e) {
                e.printStackTrace();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        connect.setText("连接");
                        Toast.makeText(SpinnerActicity.this, "连接失败", Toast.LENGTH_LONG).show();
                    }
                });
            }
        }

    }

    //倒计时刷新数据模式方法
//    private void initView()
//    {
//        n=n+1;
//            countDownTimer =new CountDownTimer(n*5000,5000) {
//                @Override
//                public void onTick(long millisUntilFinished) {
//                        new Thread(){
//                            public void run(){
//                                try{
//                                    mConnectThread.OutPut();
//                                    mConnectThread.out.write(2);
//                                }catch (Exception e){
//                                    e.printStackTrace();
//                                }
//                            }
//                        }.start();
//                        String m=String.valueOf(n);
//                        Toast.makeText(SpinnerActicity.this, m, Toast.LENGTH_LONG).show();
//                }
//
//                @Override
//                public void onFinish() {
//                    Toast.makeText(SpinnerActicity.this, "数据传输异常", Toast.LENGTH_LONG).show();
//                }
//            };
//            countDownTimer.start();
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        if (countDownTimer != null) {
//            countDownTimer.cancel();
//            countDownTimer = null;
//        }
//    }
}