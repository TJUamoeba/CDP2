package com.example.esp_app;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;

import androidx.appcompat.app.AppCompatActivity;

public class SpinnerActicity extends AppCompatActivity implements OnItemSelectedListener {
    private Spinner spi = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        spi = (Spinner) findViewById(R.id.spi_list);
        String[] arr = {"数据栏▽","温湿数据", "烟霾数据", "火焰指数"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, arr);

        spi.setAdapter(adapter);
        spi.setOnItemSelectedListener((AdapterView.OnItemSelectedListener) this);

    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        String content=spi.getSelectedItem().toString();
        switch (content) {
            case "温湿数据": {
                Intent intent = new Intent(this, HumitureActivity.class);
                startActivity(intent);
                break;
            }

            case "烟霾数据":
            {
                Intent intent=new Intent(this,HazeActiviy.class);
                startActivity(intent);
                break;
            }

            case "火焰指数":
            {
                Intent intent=new Intent(this,FireActivity.class);
                startActivity(intent);
                break;
            }
        }
    }

    public void onNothingSelected(AdapterView<?> parent) {
    }
}


