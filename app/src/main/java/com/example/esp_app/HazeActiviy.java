package com.example.esp_app;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

public class HazeActiviy extends AppCompatActivity {

    private ImageButton but;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_haze);
        but=(ImageButton)findViewById(R.id.image_haze);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(HazeActiviy.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
    }
}
