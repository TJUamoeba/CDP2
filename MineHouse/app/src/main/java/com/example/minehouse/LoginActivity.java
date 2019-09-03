package com.example.minehouse;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.Reader;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;


public class LoginActivity extends AppCompatActivity {

    Socket socket = null;
    OutputStream outputStream = null;
    InputStream inputStream = null;
    BufferedReader bufferedReader;

    private EditText ipText, portText;
    private Button connectButton;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        connectButton = (Button) findViewById(R.id.connectButton);
        //点击链接按钮时

        connectButton.setOnClickListener(new View.OnClickListener() {
                                             @Override
                                             public void onClick(View v) {
                                                 System.out.println("Click");
                                                 if(socket == null  || !socket.isConnected()){
                                                     System.out.println("New connected thread");
                                                     ConnectThread connectThread = new ConnectThread();
                                                     connectThread.start();
                                                 }
                                                 else {
                                                     try{
                                                         SendMessage(50);
                                                     }catch (Exception e){
                                                         e.printStackTrace();
                                                     }

                                                 }

                                             }
                                         }
        );
    }

    private class ConnectThread extends Thread{
        public void run(){
            if(socket == null || !socket.isConnected()){
                try{
                    socket = new Socket("192.168.43.54",8266);
                    System.out.println("Connected to socket: 192.168.43.54 ");
                }catch (Exception e){
                    e.printStackTrace();
                    System.out.println("Connected to socket FAILED ");
                }
            }
                try {
                    SendMessage(50);
                    ReceiveThread receiveThread = new ReceiveThread();
                    receiveThread.start();
                }catch (Exception e){
                    e.printStackTrace();
                    System.out.println("Send Message FAILED ");
                }

        }
    }
    private class ReceiveThread extends Thread {
        public void run() {
            try {
                System.out.println("Start receive.");
                char[] sbuf = new char[1024];
                bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                while (true) {
                    if (bufferedReader.ready()) {
                        bufferedReader.read(sbuf, 0, 10);
                        String line = String.valueOf(sbuf);
                        System.out.println(line);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
                System.out.println("Received Message FAILED ");
            }
        }
    }

    private void SendMessage(final int i){
        new Thread(){
            public void run() {
                try {
                    socket.getOutputStream().write(i);
                    System.out.println("Send to Server: 50 ");
                } catch (IOException e) {
                    e.printStackTrace();
                    System.out.println("Received Message FAILED ");
                }
            }
        }.start();
    }
}
