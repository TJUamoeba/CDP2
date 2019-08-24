package com.example.esp_app;


import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import androidx.appcompat.app.AppCompatActivity;

public class SpinnerActicity extends AppCompatActivity {
    private Spinner spi = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        spi = (Spinner) findViewById(R.id.spi_list);
        String[] arr = {"温湿度数据", "烟霾数据", "火焰指数"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, arr);

        spi.setAdapter(adapter);
        spi.setOnItemSelectedListener((AdapterView.OnItemSelectedListener) this);
    }

    public void onItemSelect(AdapterView<?> parent, View view,int position){
        String content=parent.getItemAtPosition(position).toString();
        switch (content){
            case "温湿度数据":
                break;
            case "烟霾数据":
                break;
            case "火焰指数":
                break;
        }
    }
}


