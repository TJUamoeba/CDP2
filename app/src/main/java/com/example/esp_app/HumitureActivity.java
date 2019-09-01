package com.example.esp_app;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;

public class HumitureActivity extends SpinnerActicity {

    private ImageButton but;
    private Timer timer;
    private DataInputStream in;
    private ByteArrayOutputStream baos = new ByteArrayOutputStream();
    private EditText temtxt;
    private TimerTask timerTask;
    private EditText wattxt;
    private String data;
    private byte[] bytes = new byte[1024];
    private int n;
    private Button but_text;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humiture);
        but = (ImageButton) findViewById(R.id.image_humiture);
        temtxt = (EditText) findViewById(R.id.tem_txt);
        wattxt = (EditText) findViewById(R.id.wat_txt);
        but_text=(Button)findViewById(R.id.text_but);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(HumitureActivity.this, SpinnerActicity.class);
                startActivity(intent);
            }
        });
        but_text.setOnClickListener(new View.OnClickListener() {
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

    }




//                try{
//                    in=new DataInputStream(socket.getInputStream());
//                    while((n=in.read(bytes))!=-1){
//                        baos.write(bytes,0,n);
//                    }
//                    data=new String(baos.toByteArray());
//                }catch (IOException e){
//                    e.printStackTrace();
//                }




}