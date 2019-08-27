package com.example.esp_app;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

public class FireActivity extends AppCompatActivity {

    private ImageButton but;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fire_war);
        but=(ImageButton)findViewById(R.id.image_fire);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(FireActivity.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
    }
}
