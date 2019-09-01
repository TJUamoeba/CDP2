package com.example.esp_app;


import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;

import androidx.appcompat.app.AppCompatActivity;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Timer;
import java.util.TimerTask;

public class SpinnerActicity extends AppCompatActivity implements OnItemSelectedListener,View.OnClickListener {
    private Spinner spi = null;//下拉菜单
    private Button ledOn; //开led
    private Button ledOff;  //关led
    private Button connect; //连接按钮
    private EditText Ip;    //ip地址
    private String sip;     //字符串ip
    private EditText port;  //端口号
    private int iport;      //字符端口
    public Socket socket;  //套接字
    private EditText temtxt;
    private EditText wattxt;
    private Timer timer;
    public ConnectThread mConnectThread; //Tcp连接线程
    SharedPreferences.Editor editor;
    SharedPreferences sharedPreferences;
    public final static String Pre_IP="PREF_IP_ADDRESS";
    public final static String Pre_PORT="PREF_PORT_NUMBER";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sharedPreferences=getSharedPreferences("HTTP_HELPER_PREFS", Context.MODE_PRIVATE);
        editor=sharedPreferences.edit();

        spi = (Spinner) findViewById(R.id.spi_list);
        ledOn = (Button) findViewById(R.id.led_open_but);
        ledOff = (Button) findViewById(R.id.led_close_but);
        connect = (Button) findViewById(R.id.IP_connect_but);
        Ip = (EditText) findViewById(R.id.IP_num);
        port = (EditText) findViewById(R.id.port_num);
        temtxt = (EditText) findViewById(R.id.tem_txt);
        wattxt = (EditText) findViewById(R.id.wat_txt);

        String[] arr = {"数据栏▽", "温湿数据", "烟霾数据", "火焰指数"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, arr);
        if(socket!=null&&socket.isConnected()){
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    new Thread(){
                        public void run(){
                            try{
                                mConnectThread.OutPut();
                                mConnectThread.out.write(2);
                            }catch (Exception e){
                                e.printStackTrace();
                            }
                        }
                    }.start();
                }
            },0,1000);
        }

        spi.setAdapter(adapter);
        spi.setOnItemSelectedListener((AdapterView.OnItemSelectedListener) this);
        ledOn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(){
                    public void run(){
                        try{
                            mConnectThread.OutPut();
                            mConnectThread.out.write(0);
                        }catch (Exception e){
                            e.printStackTrace();
                        }
                    }
                }.start();

            }
        });
        ledOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(){
                    public void run(){
                        try{
                            mConnectThread.OutPut();
                            mConnectThread.out.write(1);
                        }catch (Exception e){
                            e.printStackTrace();
                        }
                    }
                }.start();

            }
        });
        connect.setOnClickListener((View.OnClickListener)this);

        Ip.setText(sharedPreferences.getString(Pre_IP,""));
        port.setText(sharedPreferences.getString(Pre_PORT,""));
    }

    @Override
    public void onClick(View view){
        //save the used Ip and port
        String IPAddress=Ip.getText().toString().trim();
        String PortNumber=port.getText().toString().trim();
        editor.putString(Pre_IP,IPAddress);
        editor.putString(Pre_PORT,PortNumber);
        editor.commit();

//        if(IPAddress.length()>0&&PortNumber.length()>0&&view.getId()==connect.getId()){
//            new HttpRequestAsyncTask(view.getContext(),IPAddress,PortNumber).execute();
//        }

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
        public void OutPut(){
            try {
                out=socket.getOutputStream();
            }catch (IOException e){
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
    //    public String sendRequest(String ipAddress, String portNumber) {
//        String serverResponse = "ERROR";
//
//        try {
//
//            HttpClient httpclient = new DefaultHttpClient(); // create an HTTP client
//            // define the URL e.g. http://myIpaddress:myport/?pin=13 (to toggle pin 13 for example)
//            URI website = new URI("http://"+ipAddress+":"+portNumber+"/?");
//            HttpGet getRequest = new HttpGet(); // create an HTTP GET object
//            getRequest.setURI(website); // set the URL of the GET request
//            HttpResponse response = httpclient.execute(getRequest); // execute the request
//            // get the ip address server's reply
//            InputStream content = null;
//            content = response.getEntity().getContent();
//            BufferedReader in = new BufferedReader(new InputStreamReader(
//                    content
//            ));
//            serverResponse = in.readLine();
//            // Close the connection
//            content.close();
//        } catch (ClientProtocolException e) {
//            // HTTP error
//            serverResponse = e.getMessage();
//            e.printStackTrace();
//        } catch (IOException e) {
//            // IO error
//            serverResponse = e.getMessage();
//            e.printStackTrace();
//        } catch (URISyntaxException e) {
//            // URL syntax error
//            serverResponse = e.getMessage();
//            e.printStackTrace();
//        }
//        // return the server's reply/response text
//        return serverResponse;
//    }
//
//    private class HttpRequestAsyncTask extends AsyncTask<Void, Void, Void> {
//        private String requestReply, ipAddress, portNumber;
//        private Context context;
//        private AlertDialog alertDialog;
//
//        public HttpRequestAsyncTask(Context context, String ipAddress, String portNumber) {
//            this.context = context;
//
//            alertDialog = new AlertDialog.Builder(this.context)
//                    .setTitle("HTTP Response From IP Address:")
//                    .setCancelable(true)
//                    .create();
//
//            this.ipAddress = ipAddress;
//            this.portNumber = portNumber;
//        }
//
//        @Override
//        protected Void doInBackground(Void... voids) {
//            alertDialog.setMessage("Data sent, waiting for reply from server...");
//            if (!alertDialog.isShowing()) {
//                alertDialog.show();
//            }
//            requestReply = sendRequest(ipAddress, portNumber);
//            return null;
//        }
//
//        @Override
//        protected void onPostExecute(Void aVoid) {
//            alertDialog.setMessage(requestReply);
//            if (!alertDialog.isShowing()) {
//                alertDialog.show(); // show dialog
//            }
//        }
//
//        @Override
//        protected void onPreExecute() {
//            alertDialog.setMessage("Sending data to server, please wait...");
//            if (!alertDialog.isShowing()) {
//                alertDialog.show();
//            }
//
//        }
//    }
}


