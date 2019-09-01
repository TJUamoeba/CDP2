package com.example.minehouse;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.net.InetAddress;
import java.net.Socket;


public class LoginActivity extends AppCompatActivity {

    Socket socket = null;
    private EditText ipText, portText;
    private Button connectButton;

    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        connectButton = (Button)findViewById(R.id.connectButton);
       //点击链接按钮时
        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //清空连接
                if(socket != null){
                    socket = null;
                }
                try{
                    //获取IP地址和端口号
                    InetAddress ipAddress = InetAddress.getByName(ipText.getText().toString());
                    int port = Integer.valueOf(portText.getText().toString());
                    socket = new Socket(ipAddress, port);
                }catch (Exception e){
                    e.printStackTrace();
                }


            }
        });
    }
}
