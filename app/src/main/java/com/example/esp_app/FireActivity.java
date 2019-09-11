package com.example.esp_app;

import android.content.Intent;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;

public class FireActivity extends AppCompatActivity {

    private ImageButton but;
    private int iffire=0;
    private TextView firetxt;
    private ImageView fireview;
    private MediaPlayer player;
    private LinearLayout linearLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fire_war);
        linearLayout=(LinearLayout)findViewById(R.id.layout);
        firetxt=(TextView)findViewById(R.id.fire_txt);
        fireview=(ImageView)findViewById(R.id.fire_view);
        but=(ImageButton)findViewById(R.id.image_fire);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(FireActivity.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });

        Intent intent=getIntent();
        Bundle bundle=intent.getExtras();
        iffire=bundle.getInt("ifFire");
//        iffire=1;
        if(iffire==1){
            firetxt.setText("请注意，有明火出现！");
            firetxt.setTextSize(25);
            firetxt.setTextColor(Color.RED);
            fireview.setBackgroundDrawable(getResources().getDrawable(R.drawable.firebut));
            linearLayout.setBackgroundDrawable(getResources().getDrawable(R.drawable.fire));
            player = MediaPlayer.create(this, R.raw.firemusic);
            try {
                player.prepare();// 同步
            } catch (IllegalStateException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            // //播放
            player.start();
        }
        else if(iffire==0){
            firetxt.setText("无明火出现");
            firetxt.setTextSize(30);
            firetxt.setTextColor(Color.BLACK);
            fireview.setBackgroundDrawable(getResources().getDrawable(R.drawable.but));
            linearLayout.setBackgroundDrawable(getResources().getDrawable(R.drawable.background));
        }
    }
}
