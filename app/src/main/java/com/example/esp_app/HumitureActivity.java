package com.example.esp_app;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;

public class HumitureActivity extends SpinnerActicity {

    private ImageButton but;
    private Timer timer;
    private DataInputStream in;
    private ByteArrayOutputStream baos = new ByteArrayOutputStream();
    private EditText temtxt;
    private EditText wattxt;
    private String data;
    private byte[] bytes=new byte[1024];
    private int n;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humiture);
        but=(ImageButton)findViewById(R.id.image_humiture);
        temtxt=(EditText)findViewById(R.id.tem_txt);
        wattxt=(EditText)findViewById(R.id.wat_txt);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(HumitureActivity.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
        timer=new Timer();
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
                try{
                    in=new DataInputStream(socket.getInputStream());
                    while((n=in.read(bytes))!=-1){
                        baos.write(bytes,0,n);
                    }
                    data=new String(baos.toByteArray());
                }catch (IOException e){
                    e.printStackTrace();
                }
            }
        },0,5000);
    }

}
