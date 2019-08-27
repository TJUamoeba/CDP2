package com.example.esp_app;


import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

public class SpinnerActicity extends AppCompatActivity implements OnItemSelectedListener,View.OnClickListener {
    private Spinner spi = null;//下拉菜单
    private Button ledOn; //开led
    private Button ledOff;  //关led
    private Button connect; //连接按钮
    private EditText Ip;    //ip地址
    private String sip;     //字符串ip
    private EditText port;  //端口号
    private int iport;      //字符端口
    private Socket socket;  //套接字
    private PrintStream out; //打印输出流
    private ConnectThread mConnectThread; //Tcp连接线程

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        spi = (Spinner) findViewById(R.id.spi_list);
        ledOn = (Button) findViewById(R.id.led_open_but);
        ledOff = (Button) findViewById(R.id.led_close_but);
        connect = (Button) findViewById(R.id.IP_connect_but);
        Ip = (EditText) findViewById(R.id.IP_num);
        port = (EditText) findViewById(R.id.port_num);

        String[] arr = {"数据栏▽", "温湿数据", "烟霾数据", "火焰指数"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, arr);

        spi.setAdapter(adapter);
        spi.setOnItemSelectedListener((AdapterView.OnItemSelectedListener) this);
        ledOn.setOnClickListener((View.OnClickListener) this);
        ledOff.setOnClickListener((View.OnClickListener)this);
        connect.setOnClickListener((View.OnClickListener)this);
    }

    @Override
    public void onClick(View view){
        switch (view.getId()){
            case R.id.IP_connect_but:{
                //建立连接
                if(Ip.getText().toString().equals("")||port.getText().toString().equals(""))
                {
                    Toast.makeText(SpinnerActicity.this,"请输入IP地址和端口号",Toast.LENGTH_LONG).show();
                }
                else
                {
                    if(socket==null||!socket.isConnected()){
                        sip=Ip.getText().toString();
                        iport=Integer.valueOf(port.getText().toString());
                        mConnectThread=new ConnectThread(sip,iport);
                        mConnectThread.start();
                    }
                    if(socket!=null&&socket.isConnected()){
                        try{
                            socket.close();
                            socket=null;
                            connect.setText("连接");
                            Toast.makeText(SpinnerActicity.this,"连接已关闭",Toast.LENGTH_LONG).show();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
                break;
            }
            case R.id.led_open_but:{
                if(out!=null){
                    out.print("0");
                    out.flush();
                }
                break;
            }
            case R.id.led_close_but:{
                if(out!=null){
                    out.print("1");
                    out.flush();
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
                Intent intent = new Intent(this, HumitureActivity.class);
                startActivity(intent);
                break;
            }

            case "烟霾数据": {
                Intent intent = new Intent(this, HazeActiviy.class);
                startActivity(intent);
                break;
            }

            case "火焰指数": {
                Intent intent = new Intent(this, FireActivity.class);
                startActivity(intent);
                break;
            }
        }
    }

    public void onNothingSelected(AdapterView<?> parent) {
    }

    private class ConnectThread extends Thread {
        private String ip;
        private int port;

        public ConnectThread(String ip, int port) {
            this.ip = ip;
            this.port = port;
        }

        @Override
        public void run() {
            try {
                socket = new Socket(ip, port);
                out = new PrintStream(socket.getOutputStream());
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
                        Toast.makeText(SpinnerActicity.this, "连接失败", Toast.LENGTH_LONG).show();
                    }
                });
            }
        }

    }
}


