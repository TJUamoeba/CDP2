package com.example.esp_app;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

public class HumitureActivity extends AppCompatActivity {

    private ImageButton but;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humiture);
        but=(ImageButton)findViewById(R.id.image_humiture);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(HumitureActivity.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
    }

}
