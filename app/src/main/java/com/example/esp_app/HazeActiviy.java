package com.example.esp_app;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import lecho.lib.hellocharts.gesture.ContainerScrollType;
import lecho.lib.hellocharts.gesture.ZoomType;
import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.model.ValueShape;
import lecho.lib.hellocharts.model.Viewport;
import lecho.lib.hellocharts.view.LineChartView;

public class HazeActiviy extends AppCompatActivity {

    private List<Line> lines =new ArrayList<>();//线数组
    private List<PointValue> HazPointValueList =new ArrayList<>();//雾霾点数据数组
    public List<PointValue> points=new ArrayList<>();//点数组
    private int position=0;
    private Timer timer;
    private Axis axisX=new Axis();//横坐标
    private Axis axisY=new Axis();//纵坐标
    public LineChartView mChartView;
    public LineChartView lineChartView;
    private LineChartData data=new LineChartData();
    private ImageButton but;
    private String[] hazedata=new String[4096];
    private int i=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_haze);
        but=(ImageButton)findViewById(R.id.image_haze);
        mChartView=(LineChartView)findViewById(R.id.haze_chart);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(HazeActiviy.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
        Intent intent=getIntent();
        Bundle bundle=intent.getExtras();
        hazedata =bundle.getStringArray("HazeData");
        i=bundle.getInt("length");
        if(timer!=null){
            timer.cancel();
            timer=null;
        }
        PrintHazeLine();
        timer=new Timer();
    }

    private Line HazeLine=new Line(HazPointValueList);
    @Override
    protected void onResume(){
        super.onResume();
        for(int j=0;j<i;j++){
            System.out.println("RT"+j);
            System.out.println(hazedata[j]);
            PointValue value1=new PointValue(position*5, Float.valueOf(hazedata[j]));
            HazPointValueList.add(value1);
            float x=value1.getX();


            HazeLine=new Line(HazPointValueList);
            HazeLine.setColor(Color.YELLOW);
            HazeLine.setCubic(true);
            HazeLine.setHasLabelsOnlyForSelected(true);
            HazeLine.setHasPoints(false);
            lines.add(HazeLine);

            data=initDatas(lines);
            lineChartView.setLineChartData(data);
            //根据横坐标变换界面显示范围
            Viewport port;
            if(x>50){
                port=initViewPort(x-50,x);
            }
            else
            {
                port=initViewPort(0,50);
            }
            lineChartView.setCurrentViewport(port);

            Viewport maxport=initMaxViewPort(x);
            lineChartView.setMaximumViewport(maxport);
            position++;
        }

//        timer.schedule(new TimerTask() {
//            @Override
//            public void run() {
//                //随机生成新点
//                PointValue value2=new PointValue(position*5,Float.valueOf(hazedata[i]));
//                HazPointValueList.add(value2);
//                float x=value2.getX();
//                //根据新点画线
//                lines.clear();
//
//                Line HazLine=new Line(HazPointValueList);
//                HazLine.setColor(Color.YELLOW);
//                HazLine.setCubic(true);
//                HazLine.setShape(ValueShape.CIRCLE);
//                HazLine.setHasLabelsOnlyForSelected(true);
//                HazLine.setHasPoints(false);
//                lines.add(HazLine);
//
//                data=initDatas(lines);
//                lineChartView.setLineChartData(data);
//                //根据横坐标变换界面显示范围
//                Viewport port;
//                if(x>50){
//                    port=initViewPort(x-50,x);
//                }
//                else
//                {
//                    port=initViewPort(0,50);
//                }
//                lineChartView.setCurrentViewport(port);
//
//                Viewport maxport=initMaxViewPort(x);
//                lineChartView.setMaximumViewport(maxport);
//                position++;
//                i++;
//            }
//        },0,5000);
    }

    public void PrintHazeLine(){
        lineChartView=(LineChartView)findViewById(R.id.haze_chart);
        //坐标轴
        axisX.setHasTiltedLabels(true);  //X坐标轴字体是斜的显示还是直的，true是斜的显示
        axisX.setTextColor(Color.BLACK);  //设置字体颜色
        axisX.setName("时间/s");  //表格名称
        axisX.setTextSize(10);//设置字体大小
        data.setAxisXBottom(axisX); //x 轴在底部
        axisX.setHasLines(true); //x 轴分割线

        // Y轴是根据数据的大小自动设置Y轴上限(在下面我会给出固定Y轴数据个数的解决方案)
        axisY.setName("烟雾/雾霾");//y轴标注
        axisY.setTextColor(Color.BLACK);
        axisY.setTextSize(10);//设置字体大小
        data.setAxisYLeft(axisY);  //Y轴设置在左边
        axisY.setHasLines(true);

        data=initDatas(null);
        lineChartView.setLineChartData(data);

        Viewport viewport=initViewPort(0,50);
        //设置行为属性，支持缩放、滑动以及平移
        lineChartView.setCurrentViewportWithAnimation(viewport);
        mChartView.setInteractive(true);//设置图表是可以交互的（拖拽，缩放等效果的前提）
        mChartView.setZoomType(ZoomType.HORIZONTAL);//设置缩放方向
        lineChartView.setScrollEnabled(true);
        lineChartView.setValueTouchEnabled(true);
        lineChartView.setFocusableInTouchMode(true);
        lineChartView.setViewportCalculationEnabled(false);
        lineChartView.setContainerScrollEnabled(true, ContainerScrollType.HORIZONTAL);
        lineChartView.startDataAnimation();
        points=new ArrayList<>();
    }

    private LineChartData initDatas(List<Line> line)
    {
        LineChartData lineChartData=new LineChartData(line);
        lineChartData.setAxisYLeft(axisY);
        lineChartData.setAxisXBottom(axisX);
        return lineChartData;
    }

    //当前显示界面区域
    private Viewport initViewPort(float left,float right)
    {
        Viewport port=new Viewport();
        port.top=60;
        port.bottom=0;
        port.left=left;
        port.right=right;
        return port;
    }

    //最大显示区域
    private Viewport initMaxViewPort(float right)
    {
        Viewport port=new Viewport();
        port.top=60;
        port.bottom=0;
        port.left=0;
        port.right=right+50;
        return port;
    }
}
